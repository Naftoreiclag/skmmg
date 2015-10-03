#ifndef ENTITY_HPP
#define ENTITY_HPP

#include "Vec3f.hpp"
#include "SFML/System.hpp"
#include "EntityProtocol.hpp"

#include "IcyPacketEntityUpdate.hpp"

namespace skm
{

class Entity {
public:
    Vec3f m_location;
    
    IcyPacketEntityUpdate* m_updatePacket;
    
public:
    typedef EntityProtocol::Handle Handle;
public:
    const Handle m_handle;

    Entity(const Handle& handle);
    virtual ~Entity();
    
    void setLocation(Vec3f location);
};

}

#endif // ENTITY_HPP
