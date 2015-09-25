#include "IcyServer.hpp"

//#include "DebugAwareness.hpp"

#ifndef NICYDEBUG
#include <iostream>
#endif

#include "IcyPacketHeartbeat.hpp"

namespace skm {
    
IcyServer::Message::Message() {
}
    
IcyServer::Message::Message(Type type, IcyProtocol::SessionId session)
: m_type(type)
, m_session(session) {}
    
IcyServer::Message::Message(Type type)
: m_type(type) {}

IcyServer::Message::~Message() {
}


IcyServer::Session::Session() {
}

IcyServer::Session::~Session() {
}
    
IcyServer::IcyServer() {
    m_lastSessionId = 1337;
}

IcyServer::~IcyServer() {
}

IcyServer::SpecificPacketPair::SpecificPacketPair() {
}

IcyServer::SpecificPacketPair::SpecificPacketPair(IcyProtocol::SessionId sessionId, IcyPacket* packet)
: sessionId(sessionId)
, packet(packet) {
}

IcyProtocol::SessionId IcyServer::nextAvailableSessionId() {
    return m_lastSessionId ++;
}

void IcyServer::initialize(IcyProtocol::Port port) {
    m_port = port;
    m_notifications.push_back(Message(Message::Type::ENTER_SIESTA_MODE));
    m_vacant = true;
    m_socket.setBlocking(true);
    m_socket.bind(m_port);
}

void IcyServer::startConnectionSustainingLoop(std::condition_variable& siestaCond, std::mutex& siestaMutex, bool& siestaNotify) {
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
                            newSession->m_session.m_localSequence = 0;
                            newSession->m_session.m_ack = 0;
                            newSession->m_session.m_ackBits = 0;
                            newSession->m_verifiedId = false;
                            newSession->m_verificationTimeout.restart();
                            newSession->m_verificationTimer.restart();
                            m_sessions.push_back(newSession);
                            
                            m_vacant_mutex.lock();
                            // If the server is vacant, then the main thread must have been sleeping, so wake it up.
                            if(m_vacant) {
                                siestaNotify = true;
                                siestaCond.notify_one();
                            }
                            m_vacant = false;
                            m_vacant_mutex.unlock();
                            m_socket.setBlocking(false);
                            
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
                                    #ifndef NICYDEBUG
                                    std::cout << "Client connected. Session id: " << sessionSearch->m_session.m_sessionId << std::endl;
                                    #endif
                                }
                                
                                // Connection now verified
                                sessionSearch->m_verifiedId = true;
                                m_notifications.push_back(Message(Message::Type::USER_JOIN, sessionSearch->m_session.m_sessionId));
                                
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
                        sessionSearch->m_clientTimeout.restart();
                        IcyPacket* receievedPacket = sessionSearch->m_session.processRawIncoming(receivedPacket);
                        
                        if(receievedPacket != nullptr) {
                            SpecificPacketPair incomingPacket;
                            incomingPacket.sessionId = sessionSearch->m_session.m_sessionId;
                            incomingPacket.packet = receievedPacket;
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
                        
                        m_notifications.push_back(Message(Message::Type::USER_LEAVE, session->m_session.m_sessionId));
                        delete session;
                        continue;
                    }
                    
                    ++ it;
                    continue;
                }
                
                // Session is verified, but too quiet
                if(session->m_clientTimeout.getElapsedTime().asMilliseconds() > IcyProtocol::s_serverTimeoutMs) {
                    #ifndef NICYDEBUG
                    std::cout << "Lost connection to client " << session->m_session.m_sessionId << std::endl;
                    #endif
                    
                    // Remove this client from the session list
                    it = m_sessions.erase(it);
                        
                    m_notifications.push_back(Message(Message::Type::USER_LEAVE, session->m_session.m_sessionId));
                    delete session;
                    continue;
                }
                
                ++ it;
            }
                        
            // If there are no sessions, then there is no reason to do anything but wait for an incoming message
            if(m_sessions.size() == 0) {
                #ifndef NICYDEBUG
                std::cout << "Last client disconnected. Setting listener to blocking mode." << std::endl;
                #endif
                
                m_notifications.push_back(Message(Message::Type::ENTER_SIESTA_MODE));
                
                m_vacant_mutex.lock();
                m_vacant = true;
                m_vacant_mutex.unlock();
                m_socket.setBlocking(true);
            }
        }
        
        // Send global outgoing packets
        {
            IcyPacket* outgoingPacket;
            bool isOutgoingPacket = m_outgoingGlobalPackets.pop_front(outgoingPacket);
            while(isOutgoingPacket) {
                for(std::list<Session*>::iterator it = m_sessions.begin(); it != m_sessions.end(); ++ it) {
                    Session* session = *it;
                    session->m_outgoingPackets.push_back(outgoingPacket);
                }
                
                isOutgoingPacket = m_outgoingGlobalPackets.pop_front(outgoingPacket);
            }
        }
        
        // Send client-specific outgoing packets
        {
            // Iterate through all addressed packets
            SpecificPacketPair pairPtr;
            bool isOutgoing = m_outgoingPackets.pop_front(pairPtr);
            while(isOutgoing) {
                IcyProtocol::SessionId sessionId = pairPtr.sessionId;
                IcyPacket* outgoingPacket = pairPtr.packet;
                
                //TODO optimize with a map
                for(std::list<Session*>::iterator it = m_sessions.begin(); it != m_sessions.end(); ++ it) {
                    Session* session = *it;
                    
                    if(session->m_session.m_sessionId == sessionId) {
                        session->m_outgoingPackets.push_back(outgoingPacket);
                        break;
                    }
                }
                
                isOutgoing = m_outgoingPackets.pop_front(pairPtr);
            }
            
            // Iterate through all connected sessions
            for(std::list<Session*>::iterator it = m_sessions.begin(); it != m_sessions.end(); ++ it) {
                Session* session = *it;
                
                // Session unverified
                if(!session->m_verifiedId) {
                    // Send verification requests
                    if(session->m_verificationTimer.getElapsedTime().asMilliseconds() > IcyProtocol::s_verifyDelayMs || !session->m_firstVerificationSent) {
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
                    IcyPacket* outgoingPacket;
                    bool isOutgoingPacket = session->m_outgoingPackets.pop_front(outgoingPacket);
                    while(isOutgoingPacket) {
                        session->m_session.sendOutgoing(outgoingPacket); // This is called appropriately
                        session->m_heartbeatTimer.restart();
                        
                        isOutgoingPacket = session->m_outgoingPackets.pop_front(outgoingPacket);
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

void IcyServer::send(IcyPacket* packet, IcyProtocol::SessionId sessionId) {
    // Do not queue if there is nobody to send it to
    {
        std::lock_guard<std::mutex> lock(m_vacant_mutex);
        if(m_vacant) {
            return;
        }
    }
    
    m_outgoingPackets.push_back(SpecificPacketPair(sessionId, packet));
    
}
void IcyServer::send(IcyPacket* packet) {
    // Do not queue if there is nobody to send it to
    {
        std::lock_guard<std::mutex> lock(m_vacant_mutex);
        if(m_vacant) {
            return;
        }
    }
    
    m_outgoingGlobalPackets.push_back(packet);
}

bool IcyServer::receive(IcyServer::Message& data) {
    return m_notifications.pop_front(data);
}
bool IcyServer::receive(IcyPacket*& packet, IcyProtocol::SessionId& sessionId) {
    SpecificPacketPair packetPair;
    if(m_incomingPackets.pop_front(packetPair)) {
        
        packet = packetPair.packet;
        sessionId = packetPair.sessionId;
        
        return true;
    }
    else {
        return false;
    }
}

}