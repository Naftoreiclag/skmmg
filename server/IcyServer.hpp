#ifndef ICYSERVER_HPP
#define ICYSERVER_HPP

#include <mutex>
#include <list>

#include "SFML/Network.hpp"

#include "IcyProtocol.hpp"
#include "IcyPacket.hpp"
#include "ThreadQueue.hpp"
#include "IcySession.hpp"

namespace skm {
    
class IcyServer {
public:
    class Message {
    public:
        enum Type {
            USER_JOIN,
            USER_LEAVE
        };
    public:
        Message();
        Message(Type type, IcyProtocol::SessionId session);
        ~Message();
        
        Type m_type;
        
        IcyProtocol::SessionId m_session;
    };

    class Session {
    public:
        Session();
        ~Session();
        
        sf::IpAddress m_address;
        IcyProtocol::Port m_port;
        
        IcySession m_session;
        
        bool m_verifiedId;
        bool m_firstVerificationSent;
        sf::Clock m_verificationTimeout;
        sf::Clock m_verificationTimer;
        sf::Clock m_clientTimeout;
        sf::Clock m_heartbeatTimer;
        
        ThreadQueue<IcyPacket*> m_outgoingPackets;
        //ThreadQueue<IcyPacket*> m_incomingPackets;
    };
private:
    sf::UdpSocket m_socket;
    bool m_running;
    IcyProtocol::Port m_port;
    std::list<Session*> m_sessions;
    IcyProtocol::SessionId m_lastSessionId;

public:
    IcyServer();
    ~IcyServer();
    
    // Prepare to accept clients; bind to port
    void initialize(IcyProtocol::Port port);
    
    // Start this method on a new thread; this will begin a loop that will sustain connections to the clients
    void startConnectionSustainingLoop();
    
    void terminate();
    
    struct SpecificPacketPair {
        SpecificPacketPair();
        SpecificPacketPair(IcyProtocol::SessionId sessionId, IcyPacket* packet);
        
        IcyProtocol::SessionId sessionId;
        IcyPacket* packet;
    };
    
    ThreadQueue<IcyPacket*> m_outgoingGlobalPackets;
    ThreadQueue<SpecificPacketPair> m_outgoingPackets;
    ThreadQueue<SpecificPacketPair> m_incomingPackets;
    
    ThreadQueue<Message> m_notifications;
    
    void send(IcyPacket* packet, IcyProtocol::SessionId sessionId);
    void send(IcyPacket* packet);
    
    bool receive(IcyServer::Message& data);
    bool receive(IcyPacket*& packet, IcyProtocol::SessionId& sessionId);
    
private:
    IcyProtocol::SessionId nextAvailableSessionId();
};

}

#endif // ICYSERVER_HPP
