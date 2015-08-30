#ifndef ICYCLIENT_HPP
#define ICYCLIENT_HPP

#include <mutex>

#include "SFML/Network.hpp"

#include "IcyPacket.hpp"
#include "IcyProtocol.hpp"
#include "IcySession.hpp"

namespace skm {

class IcyClient {
public:
    typedef IcySession::Status Status;
private:

    sf::UdpSocket m_socket;
    
    IcySession* m_session;
    std::mutex m_session_mutex;
public:
    IcyClient();
    ~IcyClient();
    
    // Add packets to this queue to have them sent to the server; IcyClient will handle their deletion
    ThreadQueue<IcyPacket*> m_outgoingPackets;
    
    // Packets receieved from the server will be added here; User is responsible for proper deletion
    ThreadQueue<IcyPacket*> m_incomingPackets;
    
    // Attempt to connect to a server and get the session id
    void initializeConnection(sf::IpAddress address, IcyProtocol::Port port);
    
    // Start this method on a new thread; this will begin a loop that will try sustain the connection with the server
    void startConnectionSustainingLoop();
    
    // Ends the loop called by startConnectionSustainingLoop; thread-safe
    void terminateConnection();
    
    Status getStatus();
};

}

#endif // ICYCLIENT_HPP
