#ifndef ICYSERVER_HPP
#define ICYSERVER_HPP

#include <condition_variable>
#include <mutex>
#include <list>

#include "SFML/Network.hpp"

#include "ThreadQueue.hpp"

#include "IcyProtocol.hpp"
#include "IcyPacket.hpp"
#include "IcySession.hpp"

namespace skm {

class IcyServer {
public:

    // Non-packet notifications sent by this class to the main thread
    class Message {
    public:
        enum Type {
            USER_JOIN, // A user joins the game
            USER_LEAVE, // A user leaves the game
            ENTER_SIESTA_MODE // Enter "siesta" mode; sleep main thread until awoken by a condition_variable
        };
    public:
        Message();
        Message(Type type, IcyProtocol::SessionId session);
        Message(Type type);
        ~Message();
        
        // What type of message this is
        Type m_type;
        
        // Only applicable for USER_JOIN and USER_LEAVE
        IcyProtocol::SessionId m_session;
    };

    // Wrapper for the symmetric parts of the icy protocol; i.e. IcySession
    // Note: This would probably have made more sense as a sub-class of IcySession
    class Session {
    public:
        Session();
        ~Session();
        
        // Client's location; exactly matches the one in m_session
        sf::IpAddress m_address;
        IcyProtocol::Port m_port;
        
        // Interface to the "lower-level" io
        IcySession m_session;
        
        // True if this client has left the "verification" stage of connection
        bool m_verifiedId;
        bool m_firstVerificationSent;
        sf::Clock m_verificationTimeout;
        sf::Clock m_verificationTimer;
        sf::Clock m_clientTimeout;
        sf::Clock m_heartbeatTimer;
        
        ThreadQueue<IcyPacket*> m_outgoingPackets;
    };
private:
    sf::UdpSocket m_socket;
    bool m_running;
    IcyProtocol::Port m_port;
    std::list<Session*> m_sessions;
    IcyProtocol::SessionId m_lastSessionId;
    
    struct SpecificPacketPair {
        SpecificPacketPair();
        SpecificPacketPair(IcyProtocol::SessionId sessionId, IcyPacket* packet);
        
        IcyProtocol::SessionId sessionId;
        IcyPacket* packet;
    };
    
    ThreadQueue<IcyPacket*> m_outgoingGlobalPackets;
    ThreadQueue<SpecificPacketPair> m_outgoingPackets;
    ThreadQueue<SpecificPacketPair> m_outgoingExceptionPackets;
    ThreadQueue<SpecificPacketPair> m_incomingPackets;
    ThreadQueue<Message> m_notifications;

    bool m_vacant;
    std::mutex m_vacant_mutex;
public:
    IcyServer();
    ~IcyServer();
    
    // Prepare to accept clients; bind to port
    void initialize(IcyProtocol::Port port);
    
    // Start this method on a new thread; this will begin a loop that will sustain connections to the clients
    void startConnectionSustainingLoop(std::condition_variable& siestaCond, std::mutex& siestaMutex, bool& siestaNotify);
    
    void terminate();
    
    void send(IcyPacket* packet); // Send packet to everyone on server
    void send(IcyPacket* packet, IcyProtocol::SessionId sessionId); // Send packet to specific user
    void sendExcept(IcyPacket* packet, IcyProtocol::SessionId sessionId); // Send packet to everyone but specific user (More useful then might be expected)
    
    bool receive(IcyServer::Message& data); // Receive message
    bool receive(IcyPacket*& packet, IcyProtocol::SessionId& sessionId); // Receive packet
    
private:
    IcyProtocol::SessionId nextAvailableSessionId();
};

}

#endif // ICYSERVER_HPP
