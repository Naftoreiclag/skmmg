#ifndef SKM_ICYPACKETENTITYSPAWN_HPP
#define SKM_ICYPACKETENTITYSPAWN_HPP

#include <string>
#include "IcyPacket.hpp"
#include "Entity.hpp"

namespace skm
{

class IcyPacketEntitySpawn : public IcyPacket {
public:
    Entity::Handle m_handle;

    IcyPacketEntitySpawn();
    IcyPacketEntitySpawn(Entity::Handle handle);
    virtual ~IcyPacketEntitySpawn();
    
    virtual ProtocolId getId() const;
    virtual bool isContinuous() const;
    
    virtual bool read(sf::Packet& data);
    virtual void write(sf::Packet& data);
};

}

#endif // SKM_ICYPACKETENTITYSPAWN_HPP
