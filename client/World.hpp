#ifndef SKM_WORLD_HPP
#define SKM_WORLD_HPP

#include <map>

#include "OgreSceneManager.h"

#include "Entity.hpp"

#include "IcyPacketEntitySpawn.hpp"
#include "IcyPacketEntityUpdate.hpp"
#include "IcyPacketPlayerJoin.hpp"

namespace skm
{

class World
{
public:
    
    typedef std::map<Entity::Handle, Entity*> EntityMap;
    
    World(Ogre::SceneManager* smgr);
    ~World();
    
    Ogre::SceneManager* const m_smgr;
    
    Entity::Handle m_localPlayerHandle;
    PlayerEntity m_localPlayer;
    
    void playerJoin(const IcyPacketPlayerJoin* data);
    void spawnEntity(const IcyPacketEntitySpawn& spawnData);
    void updateEntity(const IcyPacketEntityUpdate& updateData);
    
    void tick(float tps);
    
    Entity* getByHandle(const Entity::Handle handle);

    EntityMap m_entities;

};

}

#endif // SKM_WORLD_HPP
