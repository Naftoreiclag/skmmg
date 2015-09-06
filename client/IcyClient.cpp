#include "IcyClient.hpp"

#ifndef NICYDEBUG
#include <iostream>
#endif

#include "DebugAwareness.hpp"
#include "IcyPacketHeartbeat.hpp"


namespace skm {
        

IcyClient::IcyClient() {
    m_session = nullptr;
    m_socket.setBlocking(false);
    #ifndef NICYDEBUG
    std::cout << "Binding to port " << m_socket.getLocalPort() << std::endl;
    #endif
    m_socket.bind(m_socket.getLocalPort());
}

IcyClient::~IcyClient() {
}

void IcyClient::initializeConnection(sf::IpAddress address, IcyProtocol::Port port) {
    m_session = new IcySession();
    
    m_session->m_serverAddress = address;
    m_session->m_serverPort = port;
    m_session->m_socket = &m_socket;
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
                #ifndef NICYDEBUG
                std::cout << "Requesting connection..." << std::endl;
                #endif
                
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
                    
                    #ifndef NICYDEBUG
                    std::cout << "Session id: " << sessionId << std::endl;
                    #endif
                    
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
    
    
    m_status.serverContacted = true;
    
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
                #ifndef NICYDEBUG
                std::cout << "Verifying id..." << std::endl;
                #endif
                
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
                        #ifndef NICYDEBUG
                        std::cout << "Id verified. Now processing incoming non-handshake packets..." << std::endl;
                        #endif
                        IcyPacket* receievedPacket = m_session->processRawIncoming(receivedPacket);
                        
                        if(receievedPacket != nullptr) {
                            m_incomingPackets.push_back(receievedPacket);
                        }
                        
                        break;
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
    
    m_status.sessionVerified = true;
    
    m_status.connected = true;
    #ifndef NICYDEBUG
    std::cout << "Successfully initiated connection." << std::endl;
    #endif
}

void IcyClient::startConnectionSustainingLoop() {
    #ifndef NICYDEBUG
    std::cout << "Connection-sustaining loop began." << std::endl;
    #endif
    
    // Keep track of the time since we last sent any data
    sf::Clock heartbeatTimer;
    
    // Reset the server timeout, duh
    sf::Clock serverTimeout;
    
    m_connected = false;
    // As long as we are connected
    {
        std::lock_guard<std::mutex> lock(m_status_mutex);
        m_connected = m_status.connected;
    }
    while(m_connected) {
        
        // Send heartbeats
        if(heartbeatTimer.getElapsedTime().asMilliseconds() > IcyProtocol::s_heartbeatDelayMs) {
            heartbeatTimer.restart();
            m_session->sendOutgoing(new IcyPacketHeartbeat());
        }
        
        // Send outgoing packets
        IcyPacket* outgoingPacket;
        bool outgoingPacketPopped = m_outgoingPackets.pop_front(outgoingPacket);
        bool packetsSent = outgoingPacketPopped;
        while(outgoingPacketPopped) {
            m_session->sendOutgoing(outgoingPacket);
            outgoingPacketPopped = m_outgoingPackets.pop_front(outgoingPacket);
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
                    
                        IcyPacket* receievedPacket = m_session->processRawIncoming(receivedPacket);
                        
                        if(receievedPacket != nullptr) {
                            m_incomingPackets.push_back(receievedPacket);
                        }
                        
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
            terminateConnection();
        }
    }
}

void IcyClient::terminateConnection() {
    m_status.connected = false;
}

IcyClient::SessionStatus::SessionStatus() {
    serverContacted = false;
    sessionVerified = false;
    connected = false;
    sessionId = 0;
}

IcyClient::SessionStatus IcyClient::getStatus() {
    std::lock_guard<std::mutex> lock(m_status_mutex);
    return m_status;
}

bool IcyClient::isConnected() {
    SessionStatus status = getStatus();
    return status.connected;
}
    
}