#ifndef ICYCLIENT_HPP
#define ICYCLIENT_HPP

#include <mutex>

#include "SFML/Network.hpp"
#include "IcyPacket.hpp"

namespace skm {

class IcyClient {
public:
    typedef sf::Uint16 Port;
    
    typedef sf::Uint16 MagicNumber;
    typedef sf::Uint8 ProtocolId;
    typedef sf::Uint32 SessionId;

    static const MagicNumber s_magicNumber = 0xC0DE;
    static const sf::Int32 s_requestDelayMs = 750;
    static const sf::Int32 s_requestTimeoutMs = 10000;
    static const sf::Int32 s_verifyDelayMs = 750;
    static const sf::Int32 s_verifyTimeoutMs = 10000;
    static const sf::Int32 s_heartbeatDelayMs = 500;
    
    static const ProtocolId s_protocol_session = 0;
    static const ProtocolId s_protocol_heartbeat = 1;
    
    struct Status {
        bool serverContacted = false;
        bool sessionVerified = false;
        bool connected = false;
    }
    
private:
    SessionId m_sessionId;
    sf::UdpSocket m_socket;
    
    sf::IpAddress m_serverAddress;
    Port m_serverPort;
    
    Status m_status;
    std::mutex m_status_mutex;

public:
    IcyClient();
    ~IcyClient();
    
    ThreadQueue<IcyPacket*> m_outgoingPackets;
    
    void initializeConnection(sf::IpAddress address, Port port);
    void startConnectionSustainingLoop();
    void terminateConnection();
    
private:
    void processRaw(sf::Packet& packet);
};

}

#endif // ICYCLIENT_HPP
