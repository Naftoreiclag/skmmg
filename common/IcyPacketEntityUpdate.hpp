#ifndef SKM_ICYPACKETENTITYUPDATE_HPP
#define SKM_ICYPACKETENTITYUPDATE_HPP

#include <string>
#include "IcyPacket.hpp"
#include "EntityProtocol.hpp"
#include "Vec3f.hpp"

namespace skm
{

class IcyPacketEntityUpdate : public IcyPacket {
public:
    EntityProtocol::Handle m_handle;

    IcyPacketEntityUpdate();
    IcyPacketEntityUpdate(EntityProtocol::Handle handle);
    virtual ~IcyPacketEntityUpdate();
    
    bool m_exists = true;
    
    bool m_changeLoc = false;
    Vec3f m_loc;
    
    virtual ProtocolId getId() const;
    virtual bool isContinuous() const;
    
    virtual bool read(sf::Packet& data);
    virtual void write(sf::Packet& data);
    
    void despawn();
    void setLoc(Vec3f& loc);
};

}

#endif // SKM_ICYPACKETENTITYUPDATE_HPP
