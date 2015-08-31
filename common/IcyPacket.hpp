#ifndef ICYPACKET_HPP
#define ICYPACKET_HPP

#include "SFML/Network.hpp"

namespace skm
{

class IcyPacket {
public:
    typedef sf::Uint8 ProtocolId;
    
    static const ProtocolId s_protocol_session = 0;
    static const ProtocolId s_protocol_heartbeat = 1;
    static const ProtocolId s_protocol_chat = 2;
    
public:
    static IcyPacket* newPacketFromRaw(sf::Packet& packet);
    
public:
    virtual ProtocolId getId() const = 0;
    virtual bool isContinuous() const = 0;
    
    IcyPacket();
    virtual ~IcyPacket();
    
    // Populate member variables based on this data (icy protocol headers stripped)
    virtual bool read(sf::Packet& data) = 0;
    
    // Write to this packet the outgoing data
    virtual void write(sf::Packet& data) = 0;
};

}

#endif // ICYPACKET_HPP
