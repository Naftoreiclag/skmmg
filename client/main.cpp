#include <iostream>
#include <thread>
#include <mutex>

#include "SFML/Network.hpp"

int main(int argc, char **argv) {
    sf::UdpSocket socket;
    std::mutex test;
    
    std::cout << "hello world" << std::endl;
    
    sf::Clock clock;
    sf::Time time = sf::seconds(0.5f);
    while(true) {
        std::cout << clock.getElapsedTime().asMilliseconds() << std::endl;
        sf::sleep(time);
    }
	return 0;
}
