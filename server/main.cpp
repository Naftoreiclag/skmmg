#include <iostream>
#include <thread>

#include "SFML/Network.hpp"
#include "IcyServer.hpp"

using namespace skm;

int main(int argc, char **argv) {
	
    std::cout << "Server." << std::endl;
    
    IcyServer server;
    server.initialize(25564);
    server.startConnectionSustainingLoop();
    server.terminate();
    
	return 0;
}
