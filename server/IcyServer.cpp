#include "IcyServer.hpp"

#include "DebugAwareness.hpp"

#ifndef NICYDEBUG
#include <iostream>
#endif
#include <iostream>

#include "IcyPacketHeartbeat.hpp"

namespace skm {
        

IcyServer::Session::Session() {
    
}
IcyServer::Session::~Session() {
    
}
    
IcyServer::IcyServer() {
    m_lastSessionId = 1337;
}

IcyServer::~IcyServer() {
}

IcyProtocol::SessionId IcyServer::nextAvailableSessionId() {
    return m_lastSessionId ++;
}

void IcyServer::initialize(IcyProtocol::Port port) {
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
                IcyProtocol::Port senderPort;
                sf::Packet receivedPacket;
                sf::UdpSocket::Status receiveStatus = m_socket.receive(receivedPacket, senderAddress, senderPort);
                
                // Packet to receive
                if(receiveStatus == sf::UdpSocket::Status::Done) {
                    #ifndef NICYDEBUG
                    std::cout << "Packet receieved." << std::endl;
                    #endif
                    
                    // Magic number
                    IcyProtocol::MagicNumber magicNum;
                    receivedPacket >> magicNum;
                    
                    // This packet relates to an incoming new connection
                    if(magicNum == IcyProtocol::s_magicHandshake) {
                        // Get session id from client
                        IcyProtocol::SessionId sessionId;
                        receivedPacket >> sessionId;
                        
                        // Locate a matching session
                        Session* sessionSearch = nullptr;
                        for(std::list<Session*>::iterator it = m_sessions.begin(); it != m_sessions.end(); ++ it) {
                            Session* session = *it;
                            
                            if(session->m_address == senderAddress && session->m_port == senderPort) {
                                sessionSearch = session;
                                break;
                            }
                        }
                        
                        // This is a request
                        if(sessionId == IcyProtocol::s_sessionRequestId) {
                            // Match found, client already registered
                            if(sessionSearch != nullptr) {
                                continue;
                            }
                            #ifndef NICYDEBUG
                            std::cout << "New client is requesting connection..." << std::endl;
                            #endif
                            
                            // Register new client
                            Session* newSession = new Session();
                            newSession->m_address = senderAddress;
                            newSession->m_port = senderPort;
                            newSession->m_session.m_serverAddress = senderAddress;
                            newSession->m_session.m_serverPort = senderPort;
                            newSession->m_session.m_sessionId = nextAvailableSessionId();
                            newSession->m_session.m_socket = &m_socket;
                            newSession->m_verifiedId = false;
                            newSession->m_verificationTimeout.restart();
                            newSession->m_verificationTimer.restart();
                            m_sessions.push_back(newSession);
                            
                            #ifndef NICYDEBUG
                            std::cout << "New session created for client." << std::endl;
                            #endif
                            
                            continue;
                        }
                        
                        // This is a verification
                        else {
                            #ifndef NICYDEBUG
                            std::cout << "Client verifying session id." << std::endl;
                            #endif
                            // Client has not been registered yet
                            if(sessionSearch == nullptr) {
                                #ifndef NICYDEBUG
                                std::cout << "No matching session could be found." << std::endl;
                                #endif
                                continue;
                            }
                            
                            // Client sent correct id
                            if(sessionSearch->m_session.m_sessionId == sessionId) {
                                #ifndef NICYDEBUG
                                std::cout << "Correct session id receieved." << std::endl;
                                #endif
                                
                                if(!sessionSearch->m_verifiedId) {
                                    std::cout << "Client connected. Session id: " << sessionSearch->m_session.m_sessionId << std::endl;
                                }
                                
                                // Connection now verified
                                sessionSearch->m_verifiedId = true;
                                
                                sessionSearch->m_clientTimeout.restart();
                                sessionSearch->m_heartbeatTimer.restart();
                            }
                        }
                    }
                    
                    // This is a normal packet
                    else if(magicNum == IcyProtocol::s_magicNumber) {
                        // Find client
                        Session* sessionSearch = nullptr;
                        for(std::list<Session*>::iterator it = m_sessions.begin(); it != m_sessions.end(); ++ it) {
                            Session* session = *it;
                            
                            if(session->m_address == senderAddress && session->m_port == senderPort) {
                                sessionSearch = session;
                                break;
                            }
                        }
                        
                        // Client unknown
                        if(sessionSearch == nullptr) {
                            continue;
                        }
                        
                        // Process normal packet!
                        #ifndef NICYDEBUG
                        std::cout << "Processing normal packet..." << std::endl;
                        #endif
                        sessionSearch->m_clientTimeout.restart();
                        IcyPacket* receievedPacket = sessionSearch->m_session.processRawIncoming(receivedPacket);
                        
                        if(receievedPacket != nullptr) {
                            SpecificPacketPair incomingPacket;
                            incomingPacket.first = sessionSearch->m_session.m_sessionId;
                            incomingPacket.second = receievedPacket;
                            m_incomingPackets.push_back(incomingPacket);
                        }
                    }
                    
                    // Wrong magic number
                    else {
                        continue;
                    }
                }
                
                // No packet to receive
                else {
                    packetsToReceive = false;
                    break;
                }
            }
        }
        
        // Send global outgoing packets
        {
            IcyPacket** outgoingPacketPtr = m_outgoingGlobalPackets.pop_front();
            while(outgoingPacketPtr != nullptr) {
                IcyPacket* outgoingPacket = *outgoingPacketPtr;
                
                for(std::list<Session*>::iterator it = m_sessions.begin(); it != m_sessions.end(); ++ it) {
                    Session* session = *it;
                    session->m_outgoingPackets.push_back(outgoingPacket);
                }
                
                outgoingPacketPtr = m_outgoingGlobalPackets.pop_front();
            }
        }
        
        // Remove timed-out clients
        {
            std::list<Session*>::iterator it = m_sessions.begin();
            while(it != m_sessions.end()) {
                Session* session = *it;
                
                // Session is unverified
                if(!session->m_verifiedId) {
                    if(session->m_verificationTimeout.getElapsedTime().asMilliseconds() > IcyProtocol::s_verifyTimeoutMs) {
                        #ifndef NICYDEBUG
                        std::cout << "Client " << session->m_session.m_sessionId << " did not verify session id in time. Ending session..." << std::endl;
                        #endif
                        
                        // Remove this client from the session list
                        it = m_sessions.erase(it);
                        delete session;
                        continue;
                    }
                    
                    ++ it;
                    continue;
                }
                
                // Session is verified, but too quiet
                if(session->m_clientTimeout.getElapsedTime().asMilliseconds() > IcyProtocol::s_serverTimeoutMs) {
                    #ifndef NICYDEBUG
                    std::cout << "Connection of client " << session->m_session.m_sessionId << " has been lost. Ending session..." << std::endl;
                    #endif
                    
                    // Remove this client from the session list
                    it = m_sessions.erase(it);
                    delete session;
                    continue;
                }
                
                ++ it;
            }
        }
        
        // Send client-specific outgoing packets
        {
            // Iterate through all addressed packets
            SpecificPacketPair* pairPtr = m_outgoingPackets.pop_front();
            while(pairPtr != nullptr) {
                IcyProtocol::SessionId sessionId = pairPtr->first;
                IcyPacket* outgoingPacket = pairPtr->second;
                
                //TODO optimize with a map
                for(std::list<Session*>::iterator it = m_sessions.begin(); it != m_sessions.end(); ++ it) {
                    Session* session = *it;
                    
                    if(session->m_session.m_sessionId == sessionId) {
                        session->m_outgoingPackets.push_back(outgoingPacket);
                        break;
                    }
                }
                
                pairPtr = m_outgoingPackets.pop_front();
            }
            
            // Iterate through all connected sessions
            for(std::list<Session*>::iterator it = m_sessions.begin(); it != m_sessions.end(); ++ it) {
                Session* session = *it;
                
                // Session unverified
                if(!session->m_verifiedId) {
                    // Send verification requests
                    if(session->m_verificationTimer.getElapsedTime().asMilliseconds() > IcyProtocol::s_verifyDelayMs || !session->m_firstVerificationSent) {
                        #ifndef NICYDEBUG
                        std::cout << "Sending client session id..." << std::endl;
                        #endif
                        session->m_verificationTimer.restart();
                        
                        // Send verification
                        sf::Packet verification;
                        
                        verification << IcyProtocol::s_magicHandshake;
                        verification << session->m_session.m_sessionId;
                        
                        m_socket.send(verification, session->m_address, session->m_port);
        
                        session->m_firstVerificationSent = true;
                        continue;
                    }
                    
                    continue;
                }
                
                // Session has been verified; treat as normal...
                else {
                    // Manage outgoing packets specific to this session
                    IcyPacket** outgoingPacketPtr = session->m_outgoingPackets.pop_front();
                    while(outgoingPacketPtr != nullptr) {
                        #ifndef NICYDEBUG
                        std::cout << "Sending packet to client " << session->m_session.m_sessionId << std::endl;
                        #endif
                        IcyPacket* outgoingPacket = *outgoingPacketPtr;
                        session->m_session.sendOutgoing(outgoingPacket); // This is called appropriately
                        
                        outgoingPacketPtr = session->m_outgoingPackets.pop_front();
                        session->m_heartbeatTimer.restart();
                    }
                    
                    // If we have not sent this client any packets within a certain time, send a dummy (heartbeat) packet to keep the connection alive
                    if(session->m_heartbeatTimer.getElapsedTime().asMilliseconds() > IcyProtocol::s_heartbeatDelayMs) {
                        session->m_outgoingPackets.push_back(new IcyPacketHeartbeat());
                    }
                }
            }
        }
    }
}

void IcyServer::terminate() {
    
    m_running = false;
    
}

}
