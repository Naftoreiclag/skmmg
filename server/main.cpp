#include <iostream>
#include <thread>

#include "SFML/Network.hpp"
#include "IcyServer.hpp"
#include "IcyPacketChat.hpp"
#include "IcyPacketPlayerJoin.hpp"

#include "World.hpp"

using namespace skm;

int main(int argc, char **argv) {
    std::cout << "SKMMG Server" << std::endl;
    
    
    IcyServer server;
    server.initialize(25564);
    std::thread clientThread(&IcyServer::startConnectionSustainingLoop, &server);
    
    World world;
    world.m_server = &server;
    
    sf::Clock broadcastClock;
    while(true) {
        
        IcyServer::Message msg;
        bool isMsg = server.m_notifications.pop_front(msg);
        while(isMsg) {
            switch(msg.m_type) {
                case IcyServer::Message::Type::USER_JOIN: {
                    std::cout << "User " << msg.m_session << " joined!" << std::endl;
                    world.spawnPlayer(msg.m_session);
                    break;
                }
                case IcyServer::Message::Type::USER_LEAVE: {
                    std::cout << "User " << msg.m_session << " left!" << std::endl;
                    break;
                }
                default: {
                    break;
                }
            }
            isMsg = server.m_notifications.pop_front(msg);
        }
        
        IcyServer::SpecificPacketPair data;
        bool isData = server.m_incomingPackets.pop_front(data);
        while(isData) {
            
            if(data.packet->getId() == IcyPacket::s_protocol_chat) {
                std::cout << "Chat packet" << std::endl;
                IcyPacketChat* chatPack = (IcyPacketChat*) data.packet;
                
                std::cout << data.sessionId << ":" << chatPack->m_message << std::endl;
                
                //server.m_outgoingGlobalPackets.push_back(new IcyPacketChat(chatPack->m_message));
            }
            
            delete data.packet;
            isData = server.m_incomingPackets.pop_front(data);
        }
        
        if(broadcastClock.getElapsedTime().asMilliseconds() >= 5000) {
            
            IcyPacketChat* chatPack = new IcyPacketChat("yolo");
            //IcyPacketPlayerJoin* chatPack = new IcyPacketPlayerJoin(315);
            
            server.m_outgoingGlobalPackets.push_back(chatPack);
            
            broadcastClock.restart();
        }
    }
    
	return 0;
}
