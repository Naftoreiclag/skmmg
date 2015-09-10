#ifndef ICYPACKET_HPP
#define ICYPACKET_HPP

#include "SFML/Network.hpp"

namespace skm
{

class IcyPacket {
public:
    typedef sf::Uint8 ProtocolId;
    
    static const ProtocolId s_protocol_session = 0;
    static const ProtocolId s_protocol_heartbeat = 7;
    static const ProtocolId s_protocol_chat = 2;
    static const ProtocolId s_protocol_playerJoin = 3;
    
public:
    static IcyPacket* newPacketFromRaw(sf::Packet& packet);

private:
    sf::Uint32 m_numGrabbers;

public:
    virtual ProtocolId getId() const = 0;
    virtual bool isContinuous() const = 0;
    
    IcyPacket();
    virtual ~IcyPacket();
    
    void grab();
    void grab(sf::Uint32 numGrabbers);
    void drop();
    void dropNoDelete();
    
    // Populate member variables based on this data (icy protocol headers stripped)
    virtual bool read(sf::Packet& data) = 0;
    
    // Write to this packet the outgoing data
    virtual void write(sf::Packet& data) = 0;
};

}

#endif // ICYPACKET_HPP
