#ifndef ICYPACKETHEARTBEAT_HPP
#define ICYPACKETHEARTBEAT_HPP

#include "IcyPacket.hpp"

namespace skm
{

class IcyPacketHeartbeat : public IcyPacket {
public:
    IcyPacketHeartbeat();
    virtual ~IcyPacketHeartbeat();
    
    virtual ProtocolId getId() const;
    virtual bool isContinuous() const;
    
    virtual bool read(sf::Packet& data);
    virtual void write(sf::Packet& data);
};

}

#endif // ICYPACKETHEARTBEAT_HPP
