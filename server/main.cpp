#include <iostream>
#include <thread>

#include "SFML/Network.hpp"
#include "IcyServer.hpp"
#include "IcyPacketChat.hpp"
#include "IcyPacketPlayerJoin.hpp"

#include "World.hpp"

using namespace skm;

int main(int argc, char **argv) {
    // Hello world
    std::cout << "SKMMG Server" << std::endl;
    
    // Create new server, initialize
    IcyServer server;
    server.initialize(25564);
    std::thread clientThread(&IcyServer::startConnectionSustainingLoop, &server);
    
    // 
    World world;
    world.m_server = &server;
    
    // Run for eternity
    while(true) {
        
        // Get messages from the server
        {
            IcyServer::Message data;
            bool isData = server.receive(data);
            while(isData) {
                switch(data.m_type) {
                    case IcyServer::Message::Type::USER_JOIN: {
                        std::cout << "User " << data.m_session << " joined!" << std::endl;
                        world.spawnPlayer(data.m_session);
                        break;
                    }
                    case IcyServer::Message::Type::USER_LEAVE: {
                        std::cout << "User " << data.m_session << " left!" << std::endl;
                        world.despawnPlayer(data.m_session);
                        break;
                    }
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
    }
    
	return 0;
}
