#include <iostream>
#include <thread>

#include "SFML/Network.hpp"
#include "IcyServer.hpp"
#include "IcyPacketChat.hpp"

#include "World.hpp"

using namespace skm;

int main(int argc, char **argv) {
    std::cout << "SKMMG Server" << std::endl;
    
    World world;
    
    IcyServer server;
    server.initialize(25564);
    std::thread clientThread(&IcyServer::startConnectionSustainingLoop, &server);
    
    sf::Clock clock;
    sf::Time time = sf::seconds(0.5f);
    while(true) {
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
        //sf::sleep(time);
    }
    
	return 0;
}
