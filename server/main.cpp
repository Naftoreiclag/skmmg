#include <iostream>
#include <thread>

#include "SFML/Network.hpp"
#include "IcyServer.hpp"

using namespace skm;

int main(int argc, char **argv) {
	
    std::cout << "Server." << std::endl;
    
    IcyServer server;
    server.initialize(25564);
    std::thread clientThread(&IcyServer::startConnectionSustainingLoop, &server);
    
    sf::Clock clock;
    sf::Time time = sf::seconds(0.5f);
    while(true) {
        
        // Something
    }
    
	return 0;
}
