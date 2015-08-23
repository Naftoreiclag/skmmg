#include "IcyClient.hpp"

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
                verifySessionId << s_protocol_session;
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
                    ProtocolId protocolId;
                    SessionId sessionId;
                    
                    receivedPacket >> magicNum;
                    receivedPacket >> protocolId;
                    receivedPacket >> sessionId;
                    
                    if(magicNum == s_magicNumber) {
                        if(protocolId == s_protocol_session) {
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
                verifySessionId << s_protocol_session;
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
                    processRaw(receivedPacket);
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
    sf::Clock heartbeatTimer;
    while(m_status.connected) {
        if(heartbeatTimer.getElapsedTime().asMilliseconds() > s_heartbeatDelayMs) {
            heartbeatTimer.restart();
            
            // Send heartbeat
        }
        
        IcyPacket** outgoingPacket = m_outgoingPackets.pop_front();
        if(outgoingPacket != nullptr) {
            
            // Send outgoing
        }
        
        sf::IpAddress senderAddress;
        Port senderPort;
        sf::Packet receivedPacket;
        sf::UdpSocket::Status receiveStatus = m_socket.receive(receivedPacket, senderAddress, senderPort);
        
        if(receiveStatus == sf::UdpSocket::Status::Done) {
            if(senderPort == m_serverPort && senderAddress == m_serverAddress) {
                processRaw(receivedPacket);
            }
            
            else {
                // Got a response from elsewhere??
            }
        }
    }
}

void IcyClient::terminateConnection() {
    m_connected_mutex.lock();
    m_connected = false;
    m_connected_mutex.unlock();
    
    m_outgoingPackets.clear();
}

void IcyClient::processRaw(sf::Packet& packet) {
    
}

}