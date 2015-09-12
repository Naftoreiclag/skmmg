#ifndef ICYPACKETDISCONNECT_HPP
#define ICYPACKETDISCONNECT_HPP

#include "IcyPacket.hpp"

namespace skm
{

class IcyPacketDisconnect : public IcyPacket {
public:
    IcyPacketDisconnect();
    virtual ~IcyPacketDisconnect();
    
    virtual ProtocolId getId() const;
    virtual bool isContinuous() const;
    
    virtual bool read(sf::Packet& data);
    virtual void write(sf::Packet& data);
};

}

#endif // ICYPACKETDISCONNECT_HPP
