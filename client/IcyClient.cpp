#include "IcyClient.hpp"

#include "IcyPacketHeartbeat.hpp"

namespace skm {
        

IcyClient::IcyClient() {
    m_session = nullptr;
    m_socket.setBlocking(false);
    m_socket.bind(m_socket.getLocalPort());
}

IcyClient::~IcyClient() {
}

void IcyClient::initializeConnection(sf::IpAddress address, IcyProtocol::Port port) {
    m_session = new IcySession();
    
    m_session->m_serverAddress = address;
    m_session->m_serverPort = port;
    m_session->m_socket = &m_socket;
    m_session->m_incomingPackets = &m_incomingPackets;
    m_session->m_outgoingPackets = &m_outgoingPackets;
    
    bool requestTimedOut = false;
    {
        sf::Clock resendTimer;
        sf::Clock timeoutTimer;
        bool firstRequest = true;
        while(!requestTimedOut) {
            if(resendTimer.getElapsedTime().asMilliseconds() > IcyProtocol::s_requestDelayMs || firstRequest) {
                resendTimer.restart();
                
                sf::Packet verifySessionId;
                
                verifySessionId << IcyProtocol::s_magicHandshake;
                verifySessionId << IcyProtocol::s_sessionRequestId;
                
                m_socket.send(verifySessionId, m_session->m_serverAddress, m_session->m_serverPort);
                
                firstRequest = false;
            }
            
            sf::IpAddress senderAddress;
            IcyProtocol::Port senderPort;
            sf::Packet receivedPacket;
            sf::UdpSocket::Status receiveStatus = m_socket.receive(receivedPacket, senderAddress, senderPort);
            
            if(receiveStatus == sf::UdpSocket::Status::Done) {
                if(senderPort == m_session->m_serverPort && senderAddress == m_session->m_serverAddress) {
                    
                    IcyProtocol::MagicNumber magicNum;
                    IcyProtocol::SessionId sessionId;
                    
                    receivedPacket >> magicNum;
                    receivedPacket >> sessionId;
                    
                    if(magicNum == IcyProtocol::s_magicHandshake) {
                        m_session->m_sessionId = sessionId;
                        requestTimedOut = false;
                        break;
                    }
                    else {
                        // Wrong magic number??
                    }
                }
                
                else {
                    // Got a response from elsewhere??
                }
            }
            
            if(timeoutTimer.getElapsedTime().asMilliseconds() > IcyProtocol::s_requestTimeoutMs) {
                requestTimedOut = true;
                break;
            }
        }
    }
    
    // Error, could not contact server
    if(requestTimedOut) {
        delete m_session;
        m_session = nullptr;
        return;
    }
    
    m_session->m_status.serverContacted = true;
    
    m_session->m_localSequence = 0;
    m_session->m_ack = 0;
    m_session->m_ackBits = 0;
    
    bool responseTimedOut = false;
    {
        sf::Clock resendTimer;
        sf::Clock timeoutTimer;
        bool firstVerify = true;
        while(!responseTimedOut) {
            if(resendTimer.getElapsedTime().asMilliseconds() > IcyProtocol::s_verifyDelayMs || firstVerify) {
                resendTimer.restart();
                
                sf::Packet verifySessionId;
                
                verifySessionId << IcyProtocol::s_magicHandshake;
                verifySessionId << m_session->m_sessionId;
                
                m_socket.send(verifySessionId, m_session->m_serverAddress, m_session->m_serverPort);
                
                firstVerify = false;
            }
            
            sf::IpAddress senderAddress;
            IcyProtocol::Port senderPort;
            sf::Packet receivedPacket;
            sf::UdpSocket::Status receiveStatus = m_socket.receive(receivedPacket, senderAddress, senderPort);
            
            if(receiveStatus == sf::UdpSocket::Status::Done) {
                if(senderPort == m_session->m_serverPort && senderAddress == m_session->m_serverAddress) {
                    // Check magic number
                    IcyProtocol::MagicNumber magicNum;
                    receivedPacket >> magicNum;
                    
                    // Magic number is correct
                    if(magicNum == IcyProtocol::s_magicNumber) {
                    
                        m_session->processRawIncoming(receivedPacket);
                        
                    }
                    
                    // Magic number unknown
                    else {
                        // Wrong magic number??
                    }
                }
                
                else {
                    // Got a response from elsewhere??
                }
            }
            
            if(timeoutTimer.getElapsedTime().asMilliseconds() > IcyProtocol::s_verifyTimeoutMs) {
                responseTimedOut = true;
                break;
            }
        }
    }
    
    // Error, session id could not be verified
    if(responseTimedOut) {
        delete m_session;
        m_session = nullptr;
        return;
    }
    
    m_session->m_status.sessionVerified = true;
    
    m_session->m_status.connected = true;
}

void IcyClient::startConnectionSustainingLoop() {
    
    // Keep track of the time since we last sent any data
    sf::Clock heartbeatTimer;
    
    // Reset the server timeout, duh
    sf::Clock serverTimeout;
    
    // As long as we are connected
    while(m_session->m_status.connected) {
        
        // Send heartbeats
        if(heartbeatTimer.getElapsedTime().asMilliseconds() > IcyProtocol::s_heartbeatDelayMs) {
            heartbeatTimer.restart();
            m_session->sendOutgoing(new IcyPacketHeartbeat());
        }
        
        // Send outgoing packets
        IcyPacket** outgoingPacketPtr = m_outgoingPackets.pop_front();
        bool packetsSent = outgoingPacketPtr != nullptr;
        while(outgoingPacketPtr != nullptr) {
            IcyPacket* outgoingPacket = *outgoingPacketPtr;
            m_session->sendOutgoing(outgoingPacket);
            outgoingPacketPtr = m_outgoingPackets.pop_front();
        }
        if(packetsSent) {
            heartbeatTimer.restart();
        }
        
        // Receive incoming packets
        bool packetsToReceive = true;
        while(packetsToReceive) {
            sf::IpAddress senderAddress;
            IcyProtocol::Port senderPort;
            sf::Packet receivedPacket;
            sf::UdpSocket::Status receiveStatus = m_socket.receive(receivedPacket, senderAddress, senderPort);
            
            // Packet to receive
            if(receiveStatus == sf::UdpSocket::Status::Done) {
                // Data was from server
                if(senderPort == m_session->m_serverPort && senderAddress == m_session->m_serverAddress) {
                    // Check magic number
                    IcyProtocol::MagicNumber magicNum;
                    receivedPacket >> magicNum;
                    
                    // Magic number is correct
                    if(magicNum == IcyProtocol::s_magicNumber) {
                    
                        m_session->processRawIncoming(receivedPacket);
                        serverTimeout.restart();
                        
                    }
                    
                    // Magic number unknown
                    else {
                        // Wrong magic number??
                    }
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
        if(serverTimeout.getElapsedTime().asMilliseconds() > IcyProtocol::s_serverTimeoutMs) {
            // Too long; terminate connection
            //terminateConnection();
        }
    }
}

IcyClient::SessionStatus IcyClient::getStatus() {
    SessionStatus status;
    if(m_session == nullptr) {
        status.connected = false;
        status.serverContacted = false;
        status.sessionVerified = false;
        status.sessionId = 0;
    }
    else {
        status.connected = m_session->m_status.connected;
        status.serverContacted = m_session->m_status.serverContacted;
        status.sessionVerified = m_session->m_status.sessionVerified;
        status.sessionId = m_session->m_sessionId;
    }
    return status;
}
    
}