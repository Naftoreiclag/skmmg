#include <iostream>
#include <thread>
#include <mutex>

#include "SFML/Network.hpp"

#include "IcyClient.hpp"

using namespace skm;

int main(int argc, char **argv) {
    IcyClient client;
    
    sf::IpAddress serverAddress = sf::IpAddress::LocalHost;
    IcyClient::Port serverPort = 25564;
    
    client.initializeConnection(serverAddress, serverPort);
    client.startConnectionSustainingLoop();
    //std::thread clientThread(&IcyClient::startConnectionSustainingLoop, client);
    client.terminateConnection();
    
    sf::Clock clock;
    sf::Time time = sf::seconds(0.5f);
    while(true) {
        std::cout << clock.getElapsedTime().asMilliseconds() << std::endl;
        sf::sleep(time);
    }
	return 0;
}
