#include "IcyServer.hpp"

#include "IcyPacketHeartbeat.hpp"

namespace skm {
        

IcyServer::IcyServer() {
}

IcyServer::~IcyServer() {
}

SessionId IcyServer::nextAvailableSessionId() {
    return 1337;
}

void IcyServer::initialize(Port port) {
    m_port = port;
    m_socket.setBlocking(false);
    m_socket.bind(m_port);
}

void IcyServer::startConnectionSustainingLoop() {
    // As long as we are connected
    while(m_running) {
        // Receive incoming packets
        {
            bool packetsToReceive = true;
            while(packetsToReceive) {
                sf::IpAddress senderAddress;
                Port senderPort;
                sf::Packet receivedPacket;
                sf::UdpSocket::Status receiveStatus = m_socket.receive(receivedPacket, senderAddress, senderPort);
                
                // Packet to receive
                if(receiveStatus == sf::UdpSocket::Status::Done) {
                    // Magic number
                    MagicNumber magicNum;
                    receivedPacket >> magicNum;
                    
                    // This packet relates to an incoming new connection
                    if(magicNum == s_magicHandshake) {
                        // Get session id from client
                        SessionId session;
                        receivedPacket >> session;
                        
                        // Locate a matching session
                        Session* sessionSearch = nullptr;
                        for(std::list<Session>::iterator it = m_sessions.begin(); it != m_sessions.end(); ++ it) {
                            Session& session = *it;
                            
                            if(session.m_address == senderAddress && session.m_port == senderPort) {
                                sessionSearch = &session;
                                break;
                            }
                        }
                        
                        // This is a request
                        if(session == s_sessionRequestId) {
                            // Match found, client already registered
                            if(sessionSearch != nullptr) {
                                continue;
                            }
                            
                            // Register new client
                            Session newSession;
                            newSession.m_address = senderAddress;
                            newSession.m_port = senderPort;
                            newSession.m_sessionId = nextAvailableSessionId();
                            newSession.m_verifiedId = false;
                            newSession.m_verificationTimeout.restart();
                            newSession.m_verificationTimer.restart();
                            m_sessions.push_back(newSession);
                            
                            continue;
                        }
                        
                        // This is a verification
                        else {
                            // Client has not been registered yet
                            if(sessionSearch == nullptr) {
                                continue;
                            }
                            
                            // Client sent correct id
                            if(sessionSearch->m_sessionId == session) {
                                // Connection now verified
                                sessionSearch->m_verifiedId = true;
                                
                                sessionSearch->m_clientTimeout.restart();
                                sessionSearch->m_heartbeatTimer.restart();
                            }
                        }
                    }
                    
                    // This is a normal packet
                    else if(magicNum == s_magicNumber) {
                        // Find client
                        Session* sessionSearch = nullptr;
                        for(std::list<Session>::iterator it = m_sessions.begin(); it != m_sessions.end(); ++ it) {
                            Session& session = *it;
                            
                            if(session.m_address == senderAddress && session.m_port == senderPort) {
                                sessionSearch = &session;
                                break;
                            }
                        }
                        
                        // Client unknown
                        if(sessionSearch == nullptr) {
                            continue;
                        }
                        
                        // Process normal packet!
                        processRaw(sessionSearch, receivedPacket);
                    }
                    
                    // Wrong magic number
                    else {
                        continue;
                    }
                }
                
                // No packet to receive
                else {
                    packetsToReceive = false;
                }
            }
        }
        
        // Send global outgoing packets
        {
            IcyPacket** outgoingPacketPtr = m_outgoingGlobalPackets.pop_front()
            while(outgoingPacketPtr != nullptr) {
                IcyPacket* outgoingPacket = *outgoingPacketPtr;
                
                for(std::list<Session>::iterator it = m_sessions.begin(); it != m_sessions.end(); ++ it) {
                    Session& session = *it;
                    
                    sendOutgoing(session, outgoingPacket);
                }
                
                outgoingPacketPtr = m_outgoingPackets.pop_front();
            }
        }
        
        // Remove timed-out clients
        {
            std::list<Session>::iterator it = m_sessions.begin();
            while(it != m_sessions.end()) {
                Session& session = *it;
                
                // Session is unverified and we have not yet receieved verification
                if(!session.m_verifiedId) {
                    if(session.m_verificationTimeout.getElapsedTime().asMilliseconds() > s_verifyTimeoutMs) {
                        // Remove this client from the session list
                        it = m_sentPackets.erase(it);
                        continue;
                    }
                    
                    continue;
                }
                
                if(session.m_clientTimeout.getElapsedTime().asMilliseconds() > s_serverTimeoutMs) {
                    // Remove this client from the session list
                    it = m_sentPackets.erase(it);
                    continue;
                }
                
                ++ it;
            }
        }
        
        // Send client-specific outgoing packets
        {
            // Iterate through all connected sessions
            for(std::list<Session>::iterator it = m_sessions.begin(); it != m_sessions.end(); ++ it) {
                Session& session = *it;
                
                // Session unverified
                if(!session.m_verifiedId) {
                    // Send verification requests
                    if(session.m_verificationTimer.getElapsedTime().asMilliseconds() > s_verifyDelayMs || !m_firstVerificationSent) {
                        session.m_verificationTimer.restart();
                        
                        // Send verification
                        sf::Packet verification;
                        
                        verification << s_magicHandshake;
                        verification << session.m_sessionId;
                        
                        m_socket.send(verification, session.m_address, session.m_port);
        
                        m_firstVerificationSent = true;
                        continue;
                    }
                    
                    continue;
                }
                
                // Session has been verified; treat as normal...
                else {
                    // Manage outgoing packets specific to this session
                    IcyPacket** outgoingPacketPtr = session.m_outgoingPackets.pop_front()
                    while(outgoingPacketPtr != nullptr) {
                        IcyPacket* outgoingPacket = *outgoingPacketPtr;
                        sendOutgoing(session, outgoingPacket);
                        
                        outgoingPacketPtr = session.m_outgoingPackets.pop_front();
                    }
                    
                    // If we have not sent this client any packets within a certain time, send a dummy (heartbeat) packet to keep the connection alive
                    if(session.m_heartbeatTimer.getElapsedTime().asMilliseconds() > s_heartbeatDelayMs) {
                        session.m_outgoingPackets.push_back(new IcyPacketHeartbeat());
                    }
                }
            }
        }
    }
}

void IcyServer::terminate() {
    
    m_running = false;
    
}

void IcyServer::processRaw(Session& session, sf::Packet& packet) {
    // Update remote sequence number and ack field
    {
        // Get when this packet was sent by the server
        SequenceNumber remoteSeq;
        packet >> remoteSeq;
        
        // This packet is more recent than the previous most recent packet
        if(remoteSeq > session.m_ack) {
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
            
            SequenceNumber shift = remoteSeq - session.m_ack;
            
            // Shift the bits, setting bit -1 to be 1 (This way, the ack represented by the full 64 bit number is properly remembered)
            session.m_ackBits <<= 1;
            session.m_ackBits |= 1;
            session.m_ackBits <<= shift - 1;
            
            // This packet is now the most recent packet
            session.m_ack = remoteSeq;
        }
        
        // This packet was supposed to be receieved earlier
        else if(remoteSeq < session.m_ack) {
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
            SequenceNumber bitPos = (session.m_ack - remoteSeq) - 1;
            
            // This packet has not been receieved before
            if((session.m_ackBits & (1 << bitPos)) == 0) {
                
                // Remember that we have now receieved it
                session.m_ackBits |= 1 << bitPos;
            }
            
            // We have already receieved this exact packet because the sequence number was set in the bitfield, so do nothing
            else {
                return false;
            }
        }
        
        // We have already receieved this exact packet because the sequence number is the same, so do nothing
        else {
            return false;
        }
    }
    
    // Get acks from the server and queue resending of lost packets
    {
        SequenceNumber firstAck;
        AckBitfield prevAcks;
        packet >> firstAck;
        packet >> prevAcks;
        std::list<SentPacket>::iterator it = session.m_sentPackets.begin();
        while(it != session.m_sentPackets.end()) {
            SentPacket& sentPacket = *it;
            
            // This is the packet referenced by the fully referenced ack
            if(sentPacket.sequence == firstAck) {
                // Delete the packet data (since there is no need to ever resend it)
                delete sentPacket.data;
                
                // Remove this packet from the list
                it = session.m_sentPackets.erase(it);
                continue;
            }
            
            // Get packet age (position in bitfield)
            SequenceNumber bitPos = (firstAck - sentPacket.sequence) - 1;
            
            // Packet is within the last [ackBitfieldSize] sent
            if(bitPos < s_ackBitfieldSize) {
                // Packet is acked in the bitfield
                if((prevAcks & (1 << bitPos)) != 0) {
                    // Delete the packet data (since there is no need to ever resend it)
                    delete sentPacket.data;
                
                    // Remove this packet from the list
                    it = session.m_sentPackets.erase(it);
                    continue;
                }
            }
            
            // Packet is too old to be in the bitfield
            else {
                // Resend packet with new id
                session.m_outgoingPackets.push_back(sentPacket.data);
                
                // Remove this packet from the list
                it = session.m_sentPackets.erase(it);
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
            return false;
        }
        
        // This packet is a heartbeat
        if(newPacket->getId() == IcyPacket::s_protocol_heartbeat) {
            // Since heartbeats are only sent in the absense of data, just delete it
            delete newPacket;
        }
        
        // This packet is not a heartbeat; someone else will need it
        else {
            session.m_incomingPackets.push_back(newPacket);
        }
    }
    
    return true;
}

void IcyServer::sendOutgoing(Session& session, IcyPacket* packet) {
    
}

}