#include "World.hpp"

#include <vector>
#include <iostream>

#include "PlayerEntity.hpp"
#include "LocalPlayerEntity.hpp"

namespace skm
{

World::World(IcyClient& client, Ogre::SceneManager* smgr)
: m_client(client)
, m_smgr(smgr)
, m_localPlayer(nullptr) {
}

World::~World() {
}

void World::reconLocUpdate(const IcyPacketReconciledLocationUpdate* data) {
    if(m_localPlayer) {
        m_localPlayer->handlePacket(data);
    }
}

void World::playerJoin(const IcyPacketPlayerJoin* data) {
    m_localPlayer = new LocalPlayerEntity(data->m_handle, m_smgr, m_client);
    m_entities[data->m_handle] = m_localPlayer;
}

void World::spawnEntity(const IcyPacketEntitySpawn& spawnData) {
    Entity* entity = new PlayerEntity(spawnData.m_handle, m_smgr);
    m_entities[spawnData.m_handle] = entity;
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

