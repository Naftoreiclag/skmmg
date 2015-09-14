#ifndef SKM_WORLD_HPP
#define SKM_WORLD_HPP

#include <map>

#include "Entity.hpp"

#include "IcyPacketEntitySpawn.hpp"
#include "IcyPacketEntityUpdate.hpp"
namespace skm
{

class World
{
public:
    
    typedef std::map<Entity::Handle, Entity*> EntityMap;
    
    World();
    ~World();
    
    void spawnEntity(const IcyPacketEntitySpawn& spawnData);
    void updateEntity(const IcyPacketEntityUpdate& updateData);
    
    Entity* getByHandle(const Entity::Handle handle);

    EntityMap m_entities;

};

}

#endif // SKM_WORLD_HPP
