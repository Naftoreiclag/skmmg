#include <iostream>
#include <thread>

#include "SFML/Network.hpp"
#include "IcyServer.hpp"

#include "IcySession.hpp"

using namespace skm;

int main(int argc, char **argv) {
	
    std::cout << "server" << std::endl;
    IcySession sessionl;
    
    sf::Clock test;
    while(true) {
        if(test.getElapsedTime().asMilliseconds() >= 500) {
            std::cout << test.getElapsedTime().asMicroseconds() << std::endl;
            test.restart();
        }
    }
    
	return 0;
}
