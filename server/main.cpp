#include <iostream>
#include <thread>

#include "SFML/Network.hpp"
#include "IcyServer.hpp"
#include "IcyPacketChat.hpp"

using namespace skm;

int main(int argc, char **argv) {
	
    std::cout << "Server." << std::endl;
    
    IcyServer server;
    server.initialize(25564);
    std::thread clientThread(&IcyServer::startConnectionSustainingLoop, &server);
    
    sf::Clock clock;
    sf::Time time = sf::seconds(0.5f);
    while(true) {
        IcyServer::SpecificPacketPair* data = server.m_incomingPackets.pop_front();
        while(data != nullptr) {
            
            if(data->packet->getId() == IcyPacket::s_protocol_chat) {
                std::cout << "Chat packet" << std::endl;
                IcyPacketChat* chatPack = (IcyPacketChat*) data->packet;
                
                std::cout << data->sessionId << ":" << chatPack->m_message << std::endl;
                
                //server.m_outgoingGlobalPackets.push_back(new IcyPacketChat(chatPack->m_message));
            }
            
            delete data->packet;
            data = server.m_incomingPackets.pop_front();
        }
        //sf::sleep(time);
    }
    
	return 0;
}
