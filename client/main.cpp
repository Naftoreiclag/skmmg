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
            std::cout << std::endl;
            std::cout << "Could not contact server!" << std::endl;
            return 0;
        }
    }
    else {
        std::cout << "Connected to server successfully." << std::endl;
        std::cout << "Session id is " << status.sessionId << std::endl;
    }
    
    
    OgreApp app(client);
    app.run();
    
	return 0;
}
