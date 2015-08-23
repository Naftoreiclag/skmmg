#include "IcyClient.hpp"

#include "IcyPacketHeartbeat.hpp"

namespace skm {
        

IcyClient::IcyClient() {
}

IcyClient::~IcyClient() {
}

void IcyClient::initializeConnection(sf::IpAddress address, Port port) {
    m_serverAddress = address;
    m_serverPort = port;
    
    m_sessionId = 0xFFFFFFFF;
    
    m_socket.setBlocking(false);
    m_socket.bind(m_socket.getLocalPort());
    
    bool requestTimedOut = false;
    {
        sf::Clock resendTimer;
        sf::Clock timeoutTimer;
        bool firstRequest = true;
        while(!requestTimedOut) {
            if(resendTimer.getElapsedTime().asMilliseconds() > s_requestDelayMs || firstRequest) {
                resendTimer.restart();
                
                sf::Packet verifySessionId;
                
                verifySessionId << s_magicNumber;
                verifySessionId << IcyPacket::s_protocol_session;
                verifySessionId << m_sessionId;
                
                m_socket.send(verifySessionId, m_serverAddress, m_serverPort);
                
                firstRequest = false;
            }
            
            sf::IpAddress senderAddress;
            Port senderPort;
            sf::Packet receivedPacket;
            sf::UdpSocket::Status receiveStatus = m_socket.receive(receivedPacket, senderAddress, senderPort);
            
            if(receiveStatus == sf::UdpSocket::Status::Done) {
                if(senderPort == m_serverPort && senderAddress == m_serverAddress) {
                    
                    MagicNumber magicNum;
                    IcyPacket::ProtocolId protocolId;
                    SessionId sessionId;
                    
                    receivedPacket >> magicNum;
                    receivedPacket >> protocolId;
                    receivedPacket >> sessionId;
                    
                    if(magicNum == s_magicNumber) {
                        if(protocolId == IcyPacket::s_protocol_session) {
                            m_sessionId = sessionId;
                            requestTimedOut = false;
                            break;
                        }
                        else {
                            // Wrong protocol id??
                        }
                    }
                    else {
                        // Wrong magic number??
                    }
                }
                
                else {
                    // Got a response from elsewhere??
                }
            }
            
            if(timeoutTimer.getElapsedTime().asMilliseconds() > s_requestTimeoutMs) {
                requestTimedOut = true;
                break;
            }
        }
    }
    
    if(requestTimedOut) {
        // Error, could not contact server
        return;
    }
    
    m_status.serverContacted = true;
    
    m_localSequence = 0;
    m_remoteSequence = 0;
    m_ack = 0;
    m_ackBits = 0;
    
    bool responseTimedOut = false;
    {
        sf::Clock resendTimer;
        sf::Clock timeoutTimer;
        bool firstVerify = true;
        while(!responseTimedOut) {
            if(resendTimer.getElapsedTime().asMilliseconds() > s_verifyDelayMs || firstVerify) {
                resendTimer.restart();
                
                sf::Packet verifySessionId;
                
                verifySessionId << s_magicNumber;
                verifySessionId << IcyPacket::s_protocol_session;
                verifySessionId << m_sessionId;
                
                m_socket.send(verifySessionId, m_serverAddress, m_serverPort);
                
                firstVerify = false;
            }
            
            sf::IpAddress senderAddress;
            Port senderPort;
            sf::Packet receivedPacket;
            sf::UdpSocket::Status receiveStatus = m_socket.receive(receivedPacket, senderAddress, senderPort);
            
            if(receiveStatus == sf::UdpSocket::Status::Done) {
                if(senderPort == m_serverPort && senderAddress == m_serverAddress) {
                    processRawIncoming(receivedPacket);
                    break;
                }
                
                else {
                    // Got a response from elsewhere??
                }
            }
            
            if(timeoutTimer.getElapsedTime().asMilliseconds() > s_verifyTimeoutMs) {
                responseTimedOut = true;
                break;
            }
        }
    }
    
    if(responseTimedOut) {
        // Error, session id could not be verified
        return;
    }
    
    m_status.sessionVerified = true;
    
    m_status.connected = true;
    
    
}

void IcyClient::startConnectionSustainingLoop() {
    
    // Keep track of the time since we last sent any data
    sf::Clock heartbeatTimer;
    
    // Reset the server timeout, duh
    sf::Clock serverTimeout;
    
    // As long as we are connected
    while(m_status.connected) {
        
        // Send heartbeats
        if(heartbeatTimer.getElapsedTime().asMilliseconds() > s_heartbeatDelayMs) {
            heartbeatTimer.restart();
            sendOutgoing(new IcyPacketHeartbeat());
        }
        
        // Send outgoing packets
        IcyPacket** outgoingPacketPtr = m_outgoingPackets.pop_front();
        bool packetsSent = outgoingPacketPtr != nullptr;
        while(outgoingPacketPtr != nullptr) {
            IcyPacket* outgoingPacket = *outgoingPacketPtr;
            sendOutgoing(outgoingPacket);
            outgoingPacketPtr = m_outgoingPackets.pop_front();
        }
        if(packetsSent) {
            heartbeatTimer.restart();
        }
        
        // Receive incoming packets
        bool packetsToReceive = true;
        while(packetsToReceive) {
            sf::IpAddress senderAddress;
            Port senderPort;
            sf::Packet receivedPacket;
            sf::UdpSocket::Status receiveStatus = m_socket.receive(receivedPacket, senderAddress, senderPort);
            
            // Packet to receive
            if(receiveStatus == sf::UdpSocket::Status::Done) {
                // Data was from server
                if(senderPort == m_serverPort && senderAddress == m_serverAddress) {
                    processRawIncoming(receivedPacket);
                    serverTimeout.restart();
                }
                
                // Data was not from server
                else {
                    // Got a response from elsewhere??
                }
            }
            
            // No packet to receive
            else {
                packetsToReceive = false;
            }
        }
        
        // Check the time passed since the last receieved packet
        if(serverTimeout.getElapsedTime().asMilliseconds() > s_serverTimeoutMs) {
            // Too long; terminate connection
            terminateConnection();
        }
    }
}

void IcyClient::terminateConnection() {
    m_status_mutex.lock();
    m_status.connected = false;
    m_status_mutex.unlock();
    
    m_outgoingPackets.clear();
    m_incomingPackets.clear();
    m_sentPackets.clear();
}

Status IcyClient::getStatus() {
    m_status_mutex.lock();
    Status copy = m_status;
    m_status_mutex.unlock();
    return copy;
}

void IcyClient::processRawIncoming(sf::Packet& packet) {
    // Check magic number
    {
        MagicNumber magicCheck;
        
        packet >> magicCheck;
        
        if(magicCheck != s_magicNumber) {
            // Wrong magic number!
            return;
        }
    }
    
    // Update remote sequence number and ack field
    {
        // Get when this packet was sent by the server
        SequenceNumber remoteSeq;
        packet >> remoteSeq;
        
        // This packet is more recent than the previous most recent packet
        if(remoteSeq > m_ack) {
            // Find how much the ack bitfield should be shifted
            /*
             * Example:
             *      Received: 9
             *      Most up-to-date: 5
             * 
             *      Position: 9 - 5 = 4
             * 
             *         0b00000000      ...     9
             *           76543210   Bit position
             *          (    1234   Acks represented before shift)
             *          (12345678   New acks represented after shift)
             * 
             */
            
            SequenceNumber shift = remoteSeq - m_ack;
            
            // Shift the bits, setting bit -1 to be 1 (This way, the ack represented by the full 64 bit number is properly remembered)
            m_ackBits << 1;
            m_ackBits |= 1;
            m_ackBits << shift - 1;
            
            // This packet is now the most recent packet
            m_ack = remoteSeq;
        }
        
        // This packet was supposed to be receieved earlier
        else if(remoteSeq < m_ack) {
            // Get where this is stored in the ack bitfield
            /*
             * Example:
             *      Received: 3
             *      Most up-to-date: 8
             * 
             *      Position: (8 - 3) - 1 = 4
             * 
             *              v Least significant bit is one packet before the most recent
             *         0b00000000      ...      8
             *           76543210   Bit position
             *          (01234567   Acks represented)
             */
            SequenceNumber bitPos = (m_ack - remoteSeq) - 1;
            
            // This packet has not been receieved before
            if((m_ackBits & (1 << bitPos)) == 0) {
                
                // Remember that we have now receieved it
                m_ackBits |= 1 << bitPos;
            }
            
            // We have already receieved this exact packet because the sequence number was set in the bitfield, so do nothing
            else {
                return;
            }
        }
        
        // We have already receieved this exact packet because the sequence number is the same, so do nothing
        else {
            return;
        }
    }
    
    // Get acks from the server and queue resending of lost packets
    {
        SequenceNumber firstAck;
        AckBitfield prevAcks;
        packet >> firstAck;
        packet >> prevAcks;
        std::list<SentPacket>::iterator it = m_sentPackets.begin();
        while(it != m_sentPackets.end()) {
            SentPacket& sentPacket = *it;
            
            // This is the packet referenced by the fully referenced ack
            if(sentPacket.sequence == firstAck) {
                // Delete the packet data (since there is no need to ever resend it)
                delete sentPacket.data;
                
                // Remove this packet from the list
                it = m_sentPackets.erase(it);
                continue;
            }
            
            // Get packet age (position in bitfield)
            SequenceNumber bitPos = (firstAck - sentPacket.sequence) - 1;
            
            // Packet is within the last [ackBitfieldSize] sent
            if(bitPos < ackBitfieldSize) {
                // Packet is acked in the bitfield
                if(prevAcks & (1 << bitPos) != 0) {
                    // Delete the packet data (since there is no need to ever resend it)
                    delete sentPacket.data;
                
                    // Remove this packet from the list
                    it = m_sentPackets.erase(it);
                    continue;
                }
            }
            
            // Packet is too old to be in the bitfield
            else {
                // Resend packet with new id
                m_outgoingPackets.push_back(sentPacket.data);
                
                // Remove this packet from the list
                it = m_sentPackets.erase(it);
                continue;
            }
            
            // Packet is not yet acked, but might be in the future
            ++ it;
        }
    }
    
    // Finally, we can process the actual data
    {
        IcyPacket* newPacket = IcyPacket::newPacketFromRaw(packet);
        
        if(newPacket == nullptr) {
            return;
        }
        
        // This packet is a heartbeat
        if(newPacket->getId() == IcyPacket::s_protocol_heartbeat) {
            // Since heartbeats are only sent in the absense of data, just delete it
            delete newPacket;
        }
        
        // This packet is not a heartbeat; someone else will need it
        else {
            m_incomingPackets.push_back(newPacket);
        }
    }
}

void IcyClient::sendOutgoing(IcyPacket* packet) {
    
    sf::Packet rawPacket;
    
    // Magic number
    rawPacket << s_magicNumber;
    
    // Local sequence
    rawPacket << m_localSequence;
    
    // Ack
    rawPacket << m_ack;
    
    // Ack bitfield
    rawPacket << m_ackBits;
    
    // Actual data
    rawPacket << packet->getId();
    packet->write(rawPacket);
    
    m_socket.send(rawPacket, m_serverAddress, m_serverPort);
    
    // Remove unverified continuous packets
    if(packet->isContinuous()) {
        std::list<SentPacket>::iterator it = m_sentPackets.begin();
        while(it != m_sentPackets.end()) {
            SentPacket& sentPacket = *it;
            
            // The previous packet does not need to be resent
            if(sentPacket.data->getId() == packet->getId()) {
                // Delete the packet data (since there is no need to ever resend it)
                delete sentPacket.data;
            
                // Remove this packet from the list
                it = m_sentPackets.erase(it);
                continue;
            }
            
            // This packet is not relevant
            ++ it;
        }
    }
    
    SentPacket metadata;
    metadata.data = packet;
    metadata.sequence = m_localSequence;
    m_sentPackets.push_back(metadata);
    
    ++ m_localSequence;
}

}