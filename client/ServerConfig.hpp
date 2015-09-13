#ifndef SKM_SERVERCONFIG_HPP
#define SKM_SERVERCONFIG_HPP

#include <vector>
#include <string>

#include "SFML/Network.hpp"

#include "IcyProtocol.hpp"

namespace skm
{

class ServerConfig {
public:
    struct Server {
        std::string name;
        sf::IpAddress address;
        IcyProtocol::Port port;
    };
public:
    typedef std::vector<Server> ServerList;

    ServerConfig();
    ~ServerConfig();
    
    ServerList m_serverList;
    
    static ServerConfig& getInstance();
};

}

#endif // SKM_SERVERCONFIG_HPP
