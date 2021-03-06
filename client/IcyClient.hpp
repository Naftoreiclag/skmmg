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
    struct SessionStatus {
        SessionStatus();
        
        bool serverContacted;
        bool sessionVerified;
        bool connected;
        
        IcyProtocol::SessionId sessionId;
    };
private:
    sf::UdpSocket m_socket;
    
    std::mutex m_status_mutex;
    SessionStatus m_status;
    IcySession* m_session;
    
    // Keep track of the time since we last sent any data
    sf::Clock m_heartbeatTimer;
    
    // Reset the server timeout, duh
    sf::Clock m_serverTimeout;
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
    
    // Alternatively, add this to the main loop
    void sustainConnection();
    void prepareTimeouts();
    
    // Ends the loop called by startConnectionSustainingLoop; thread-safe
    void terminateConnection();
    
    SessionStatus getStatus();
    bool isConnected();
    
};

}

#endif // ICYCLIENT_HPP
