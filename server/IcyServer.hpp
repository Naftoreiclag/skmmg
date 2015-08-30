#ifndef ICYSERVER_HPP
#define ICYSERVER_HPP

#include <mutex>
#include <list>

#include "SFML/Network.hpp"

#include "IcyPacket.hpp"
#include "ThreadQueue.hpp"

namespace skm {

class IcyServer {
public:
    typedef sf::Uint16 Port;
    
    typedef sf::Uint32 MagicNumber;
    typedef sf::Uint32 SequenceNumber;
    typedef sf::Uint64 AckBitfield;
    static const SequenceNumber s_ackBitfieldSize = 64;
    
    typedef sf::Uint32 SessionId;

    static const MagicNumber s_magicNumber = 0xDAFF0D11; // Magic number that make up the first bytes of every packet sent and received
    static const MagicNumber s_magicHandshake = 0xD1AB011C; // Magic number used only for initial handshakes
    
    static const sf::Int32 s_verifyDelayMs = 750; // Time between verification requests
    static const sf::Int32 s_verifyTimeoutMs = 15000; // If the client does not verify the session within this time, terminate connection
    
    static const sf::Int32 s_heartbeatDelayMs = 500; // Time between sent heartbeats
    static const sf::Int32 s_serverTimeoutMs = 15000; // If we do not receive any packets for this long, terminate connection
    
    static const SessionId s_sessionRequestId = 0xFFFFFFFF;
    
public:
    struct Session {
        SessionId m_sessionId;
        bool m_verifiedId;
        
        sf::IpAddress m_address;
        Port m_port;
        
        bool m_firstVerificationSent = false;
        sf::Clock m_verificationTimer;
        sf::Clock m_verificationTimeout;
        sf::Clock m_heartbeatTimer;
        sf::Clock m_clientTimeout;
        
        SequenceNumber m_localSequence;
        SequenceNumber m_ack;
        AckBitfield m_ackBits;
    
        struct SentPacket {
            SequenceNumber sequence;
            IcyPacket* data;
        };
        
        
        ThreadQueue<IcyPacket*> m_outgoingPackets;
        ThreadQueue<IcyPacket*> m_incomingPackets;
        
        std::list<SentPacket> m_sentPackets;
    };
private:
    sf::UdpSocket m_socket;
    Port m_port;
    
    bool m_running;
    
    std::list<Session> m_sessions;

public:
    IcyServer();
    ~IcyServer();
    
    // Prepare to accept clients; bind to port
    void initialize(Port port);
    
    // Start this method on a new thread; this will begin a loop that will sustain connections to the clients
    void startConnectionSustainingLoop();
    
    void terminate();
    
    ThreadQueue<IcyPacket*> m_outgoingGlobalPackets;
    
private:
    void sendOutgoing(Session& session, IcyPacket* packet);
    void processRaw(Session& session, sf::Packet& packet);
    
private:
    SessionId nextAvailableSessionId();
};

}

#endif // ICYSERVER_HPP
