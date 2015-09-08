#include <iostream>
#include <thread>

#include "SFML/Network.hpp"

#include "OgreApp.hpp"

#include "IcyClient.hpp"
#include "IcySession.hpp"
#include "IcyPacketChat.hpp"
#include "IcyPacketPlayerJoin.hpp"

using namespace skm;

int main(int argc, char **argv) {
    std::cout << "Client." << std::endl;
    
    /*
    OgreApp app;
    app.run();
    */
    
    sf::IpAddress serverAddress = sf::IpAddress::LocalHost;
    IcyProtocol::Port serverPort = 25564;
    std::cout << "Server info:" << serverAddress << std::endl;
    
    std::cout << "Starting connection to server..." << std::endl;
    
    IcyClient client;
    client.initializeConnection(serverAddress, serverPort);
    IcyClient::SessionStatus status = client.getStatus();
    if(!status.connected) {
        if(!status.serverContacted) {
            std::cout << "Could not contact server!" << std::endl;
            return 0;
        }
    }
    else {
        std::cout << "Connected to server successfully." << std::endl;
        std::cout << "Session id is " << status.sessionId << std::endl;
    }
    
    std::thread clientThread(&IcyClient::startConnectionSustainingLoop, &client);
    
    while(true) {
        //client.m_outgoingPackets.push_back(new IcyPacketChat(message));
        
        IcyPacket* data;
        bool dataPopped = client.m_incomingPackets.pop_front(data);
        while(dataPopped) {
            
            std::cout << "Receive" << std::endl;
            if(data->getId() == IcyPacket::s_protocol_chat) {
                IcyPacketChat* chatPack = (IcyPacketChat*) data;
                
                std::cout << "null" << ":" << chatPack->m_message << std::endl;
            }
            else if(data->getId() == IcyPacket::s_protocol_playerJoin) {
                IcyPacketPlayerJoin* playerJoin = (IcyPacketPlayerJoin*) data;
                
                std::cout << "null" << ":" << playerJoin->m_handle << std::endl;
            }
            
            delete data;
            dataPopped = client.m_incomingPackets.pop_front(data);
        }
        
        IcyClient::SessionStatus status = client.getStatus();
        if(!status.connected) {
            std::cout << "Connection lost!" << std::endl;
            break;
        }
    }
    
	return 0;
}
