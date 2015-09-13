#include <iostream>
#include <thread>
#include <vector>

#include "SFML/Network.hpp"

#include "OgreApp.hpp"

#include "ServerConfig.hpp"

#include "IcyClient.hpp"
#include "IcySession.hpp"
#include "IcyPacketChat.hpp"
#include "IcyPacketPlayerJoin.hpp"

using namespace skm;

int main(int argc, char **argv) {
    std::cout << "Client." << std::endl;
    
    ServerConfig::ServerList& servers = ServerConfig::getInstance().m_serverList;
    for(ServerConfig::ServerList::iterator it = servers.begin(); it != servers.end(); ++ it) {
        ServerConfig::Server& server = *it;
        
        std::cout << server.name << std::endl;
        std::cout << server.address << std::endl;
        std::cout << server.port << std::endl;
    }
    
    /*
    OgreApp app;
    app.run();
    */
    
    ServerConfig::Server& serverInfo = *servers.begin();
    
    sf::IpAddress& serverAddress = serverInfo.address;
    IcyProtocol::Port& serverPort = serverInfo.port;
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
                
                std::cout << "handle" << ":" << playerJoin->m_handle << std::endl;
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
