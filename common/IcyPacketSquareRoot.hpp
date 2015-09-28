#ifndef SKM_ICYPACKETSQUAREROOT_HPP
#define SKM_ICYPACKETSQUAREROOT_HPP

#include "IcyPacket.hpp"

namespace skm
{

class IcyPacketSquareRoot : public IcyPacket {
public:

    float danger;
    
    IcyPacketSquareRoot();
    virtual ~IcyPacketSquareRoot();
    
    virtual ProtocolId getId() const;
    virtual bool isContinuous() const;
    
    virtual bool read(sf::Packet& data);
    virtual void write(sf::Packet& data);
};

}

#endif // SKM_ICYPACKETSQUAREROOT_HPP
