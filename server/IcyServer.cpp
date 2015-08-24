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
                    
                    // Try find matching session
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
                
                if(!session.m_verifiedId) {
                    if(session.m_verificationTimeout.getElapsedTime().asMilliseconds() > s_verifyTimeoutMs) {
                        // Remove this client from the session list
                        it = m_sentPackets.erase(it);
                        continue;
                    }
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
            // Since there is the possibility of sessions being rem
            std::list<Session>::iterator it = m_sessions.begin();
            while(it != m_sessions.end()) {
                Session& session = *it;
                
                // Session unverified
                if(!session.m_verifiedId) {
                    if(session.m_verificationTimeout.getElapsedTime().asMilliseconds() > s_verifyTimeoutMs) {
                        // Remove this client from the session list
                        it = m_sentPackets.erase(it);
                        continue;
                    }
                    else if(session.m_verificationTimer.getElapsedTime().asMilliseconds() > s_verifyDelayMs || !m_firstVerificationSent) {
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
                
                // Manage outgoing packets
                
                
                if(session.m_heartbeatTimer.getElapsedTime().asMilliseconds() > s_heartbeatDelayMs) {
                    session.m_outgoingPackets.push_back(new IcyPacketHeartbeat());
                }
                
                ++ it;
            }
        }
        
        // Receive incoming packets
        /*
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
        */
    }
}

void IcyServer::terminate() {
    
    m_running = false;
    
}

void IcyServer::sendOutgoing(Session& session, IcyPacket* packet) {
    
}

}