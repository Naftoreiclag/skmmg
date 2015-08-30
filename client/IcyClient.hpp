#ifndef ICYCLIENT_HPP
#define ICYCLIENT_HPP

#include <mutex>
#include <list>

#include "SFML/Network.hpp"

#include "IcyPacket.hpp"
#include "ThreadQueue.hpp"

namespace skm {

class IcyClient {
public:
    typedef sf::Uint16 Port;
    
    typedef sf::Uint32 MagicNumber;
    typedef sf::Uint32 SequenceNumber;
    typedef sf::Uint64 AckBitfield;
    static const SequenceNumber s_ackBitfieldSize = 64;
    
    typedef sf::Uint32 SessionId;

    static const MagicNumber s_magicNumber = 0xDAFF0D11; // Magic number that make up the first bytes of every packet sent and received
    static const MagicNumber s_magicHandshake = 0xD1AB011C; // Magic number used only for initial handshakes
    
    static const sf::Int32 s_requestDelayMs = 750; // Time between connection requests
    static const sf::Int32 s_requestTimeoutMs = 15000; // If the server does not supply the session within this time, terminate connection
    static const sf::Int32 s_verifyDelayMs = 750; // Time between verification requests
    static const sf::Int32 s_verifyTimeoutMs = 15000; // If the server does not verify the session within this time, terminate connection
    
    static const sf::Int32 s_heartbeatDelayMs = 500; // Time between sent heartbeats
    static const sf::Int32 s_serverTimeoutMs = 15000; // If we do not receive any packets for this long, terminate connection
    
    static const SessionId s_sessionRequestId = 0xFFFFFFFF;
    
    
    struct Status {
        bool serverContacted = false;
        bool sessionVerified = false;
        bool connected = false;
    };
    
private:
    SessionId m_sessionId;
    sf::UdpSocket m_socket;
    
    sf::IpAddress m_serverAddress;
    Port m_serverPort;
    
    Status m_status;
    std::mutex m_status_mutex;
    
    SequenceNumber m_localSequence;
    SequenceNumber m_ack;
    AckBitfield m_ackBits;
    
    struct SentPacket {
        SequenceNumber sequence;
        IcyPacket* data;
    };
    
    /* When a packet is sent to the server, it is added to this list. Packets are removed and deleted when:
     *      A continuous packet is sent to replace a previous one
     *      The data was not acked by the other side, and was resent
     *      The data was not acked by the other side, but we have resent it too many times (?)
     */
    std::list<SentPacket> m_sentPackets;

public:
    IcyClient();
    ~IcyClient();
    
    // Add packets to this queue to have them sent to the server; IcyClient will handle their deletion
    ThreadQueue<IcyPacket*> m_outgoingPackets;
    
    // Packets receieved from the server will be added here; User is responsible for proper deletion
    ThreadQueue<IcyPacket*> m_incomingPackets;
    
    // Attempt to connect to a server and get the session id
    void initializeConnection(sf::IpAddress address, Port port);
    
    // Start this method on a new thread; this will begin a loop that will try sustain the connection with the server
    void startConnectionSustainingLoop();
    
    // Ends the loop called by startConnectionSustainingLoop; thread-safe
    void terminateConnection();
    
    // Return the status of the connection; good for checking if initializeConnection succeeded; thread-safe
    Status getStatus();
    
private:
    // Process raw packet data received from socket.receive; returns true if packet was handled correctly
    bool processRawIncoming(sf::Packet& packet);
    void sendOutgoing(IcyPacket* packet);
};

}

#endif // ICYCLIENT_HPP
