#include "World.hpp"

#include <vector>
#include <iostream>

#include "PlayerEntity.hpp"

namespace skm
{

World::World(Ogre::SceneManager* smgr)
: m_smgr(smgr) {
}

World::~World() {
}

void World::spawnEntity(const IcyPacketEntitySpawn& spawnData) {
    
    Entity* entity = new PlayerEntity(spawnData.m_handle, m_smgr);
    m_entities[spawnData.m_handle] = entity;
    
    std::cout << "Entity has spawned " << spawnData.m_handle << std::endl;
}
void World::updateEntity(const IcyPacketEntityUpdate& updateData) {
    EntityMap::iterator it = m_entities.find(updateData.m_handle);
    if(it == m_entities.end()) {
        // !!!
        return;
    }
    Entity* entity = it->second;
    if(!updateData.m_exists) {
        delete entity;
        m_entities.erase(it);
        return;
    }
    if(updateData.m_changeLoc) {
        entity->setLocation(updateData.m_loc);
    }
}
Entity* World::getByHandle(const Entity::Handle handle) {
    EntityMap::iterator it = m_entities.find(handle);
    
    if(it == m_entities.end()) {
        return nullptr;
    }
    else {
        return it->second;
    }
}

void World::tick(float tps) {
    for(EntityMap::iterator it = m_entities.begin(); it != m_entities.end(); ++ it) {
        Entity* entity = it->second;
        
        entity->tick(tps);
    }
}


}

