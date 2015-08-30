#include <iostream>
#include <thread>

#include "SFML/Network.hpp"

#include "IcyClient.hpp"
#include "IcySession.hpp"

using namespace skm;

int main(int argc, char **argv) {
    IcyClient client;
    
    IcySession session;
    
    sf::IpAddress serverAddress = sf::IpAddress::LocalHost;
    IcyProtocol::Port serverPort = 25564;
    
    std::cout << "Starting connection to server..." << std::endl;
    client.initializeConnection(serverAddress, serverPort);
    const IcyClient::Status status = client.getStatus();
    if(!status.connected) {
        if(!status.serverContacted) {
            std::cout << "Could not contact server!" << std::endl;
            return 0;
        }
    }
    std::thread clientThread(&IcyClient::startConnectionSustainingLoop, &client);
    //client.terminateConnection();
    
    sf::Clock clock;
    sf::Time time = sf::seconds(0.5f);
    while(true) {
        std::cout << clock.getElapsedTime().asMilliseconds() << std::endl;
        sf::sleep(time);
    }
	return 0;
}
