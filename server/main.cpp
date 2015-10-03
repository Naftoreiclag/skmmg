#include "DebugAwareness.hpp"

#include <condition_variable>
#include <mutex>
#include <iostream>
#include <thread>

#include "SFML/Network.hpp"
#include "IcyServer.hpp"
#include "IcyPacketChat.hpp"
#include "IcyPacketPlayerJoin.hpp"
#include "IcyPacketReconciledLocationUpdate.hpp"
#include "IcyPacketSquareRoot.hpp"

#include "World.hpp"
#include "Player.hpp"

using namespace skm;

int main(int argc, char **argv) {
    // Hello world
    std::cout << "SKMMG Server" << std::endl;
    
    // Create new server and initialize
    IcyServer server;
    server.initialize(25564);
    
    // Initialize siesta wakeup protocol
    std::condition_variable siestaCond;
    std::mutex siestaMutex;
    bool siestaNotify = false;
    
    // Begin thread (also begin really obvious comments)
    std::thread clientThread(&IcyServer::startConnectionSustainingLoop, &server, std::ref(siestaCond), std::ref(siestaMutex), std::ref(siestaNotify));
    
    // World
    World world;
    world.m_server = &server;
    
    // State management
    bool siestaMode = false;
    bool running = true;
    
    // Keep track of tps
    sf::Clock tpsTimer;
    
    // Main loop/thread
    while(running) {
        
        // In siesta mode
        if(siestaMode) {
            #ifndef NTHREADINGDEBUG
            std::cout << "Main thread now sleeping until user joins." << std::endl;
            #endif
            
            // Sleep until we get an atomic notification
            while(!siestaNotify) {
                std::unique_lock<std::mutex> lock(siestaMutex);
                siestaCond.wait(lock);
            }
            
            // Consume notification
            siestaNotify = false;
            
            // Leave siesta mode
            siestaMode = false;
        }
        
        // Find how long since the last tick
        float tps = tpsTimer.getElapsedTime().asSeconds();
        tpsTimer.restart();
        
        // Get internal messages
        {
            IcyServer::Message data;
            bool isData = server.receive(data);
            while(isData) {
                switch(data.m_type) {
                    // Switch server to siesta mode
                    case IcyServer::Message::Type::ENTER_SIESTA_MODE: {
                        siestaMode = true;
                        break;
                    }
                    
                    // User joined game
                    case IcyServer::Message::Type::USER_JOIN: {
                        std::cout << "User " << data.m_session << " joined!" << std::endl;
                        world.spawnPlayer(data.m_session);
                        break;
                    }
                    
                    // User left game
                    case IcyServer::Message::Type::USER_LEAVE: {
                        std::cout << "User " << data.m_session << " left!" << std::endl;
                        world.despawnPlayer(data.m_session);
                        break;
                    }
                    
                    // Unknown message
                    default: {
                        break;
                    }
                }
                
                // Iterate
                isData = server.receive(data);
            }
        }
        
        // Get packets from clients
        {
            IcyPacket* packet;
            IcyProtocol::SessionId sessionId;
            bool isData = server.receive(packet, sessionId);
            while(isData) {
                switch(packet->getId()) {
                    case IcyPacket::s_protocol_chat: {
                        std::cout << "Chat packet" << std::endl;
                        IcyPacketChat* chatPack = (IcyPacketChat*) packet;
                        
                        std::cout << sessionId << ":" << chatPack->m_message << std::endl;
                        break;
                    }
                    case IcyPacket::s_protocol_squareRoot: {
                        IcyPacketSquareRoot* sqr = (IcyPacketSquareRoot*) packet;
                        
                        std::cout << "Square: " << (sqr->danger * sqr->danger) << std::endl;
                        break;
                    }
                    case IcyPacket::s_protocol_reconciledLocationUpdate: {
                        IcyPacketReconciledLocationUpdate* locUpd = (IcyPacketReconciledLocationUpdate*) packet;
                        
                        Player* player = world.getPlayer(sessionId);
                        
                        if(player == nullptr) {
                            break;
                        }
                        
                        IcyPacketReconciledLocationUpdate* resp = player->handlePacket(locUpd);
                        
                        server.send(resp, sessionId);
                        
                        break;
                    }
                    default: {
                        break;
                    }
                }
                
                // Delete and iterator
                delete packet;
                isData = server.receive(packet, sessionId);
            }
        }
        
        // Tick world
        world.tick(tps);
    }
    
	return 0;
}
