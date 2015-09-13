#include "ServerConfig.hpp"

#include <fstream>
#include <sstream>

#include "rapidxml.hpp"

namespace skm
{

ServerConfig::ServerConfig() {
    rapidxml::xml_document<> doc;
    
    {
        std::ifstream file("servers.xml");
        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();
        std::string data(buffer.str());
        doc.parse<0>(&data[0]);
    }
    
    rapidxml::xml_node<>* rootNode = doc.first_node("serverlist");
    
    rapidxml::xml_node<>* serverNode = rootNode->first_node("server");
    while(serverNode) {
        rapidxml::xml_node<>* nameNode = serverNode->first_node("name");
        rapidxml::xml_node<>* addressNode = serverNode->first_node("address");
        rapidxml::xml_node<>* portNode = serverNode->first_node("port");
        
        Server server;
        
        if(nameNode) {
            server.name = nameNode->value();
        }
        if(addressNode) {
            server.address = sf::IpAddress(addressNode->value());
        }
        if(portNode) {
            server.port = atoi(portNode->value());
        }
        
        m_serverList.push_back(server);
        
        serverNode = serverNode->next_sibling("server");
    }
}

ServerConfig::~ServerConfig() {
}

ServerConfig& ServerConfig::getInstance() {
    static ServerConfig instance;
    return instance;
}


}

