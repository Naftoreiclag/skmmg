#include "World.hpp"

#include <vector>
#include <iostream>

#include "PlayerEntity.hpp"

namespace skm
{

World::World(IcyClient& client, Ogre::SceneManager* smgr)
: m_client(client)
, m_smgr(smgr)
, m_localPlayerHandle(0)
, m_reconLoc(client) {
}

World::~World() {
}

void World::reconLocUpdate(const IcyPacketReconciledLocationUpdate* data) {
    m_reconLoc.handlePacket(data);
}

void World::playerJoin(const IcyPacketPlayerJoin* data) {
    m_localPlayerHandle = data->m_handle;
    
    // If that entity has already spawned, try find it
    EntityMap::iterator it = m_entities.find(m_localPlayerHandle);
    if(it != m_entities.end()) {
        m_localPlayer = (PlayerEntity*) it->second;
    }
}

void World::spawnEntity(const IcyPacketEntitySpawn& spawnData) {
    Entity* entity = new PlayerEntity(spawnData.m_handle, m_smgr);
    m_entities[spawnData.m_handle] = entity;
    
    // If this is the entity that is supposed to be the player
    if(m_localPlayerHandle == spawnData.m_handle) {
        m_localPlayer = (PlayerEntity*) entity;
    }
}
void World::updateEntity(const IcyPacketEntityUpdate& updateData) {
    EntityMap::iterator it = m_entities.find(updateData.m_handle);
    if(it == m_entities.end()) {
        // !!!
        return;
    }
    Entity* entity = it->second;
    
    if(m_localPlayerHandle) {
        if(m_localPlayer == entity) {
            // ???
        }
    }
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
    m_reconLoc.tick();
    for(EntityMap::iterator it = m_entities.begin(); it != m_entities.end(); ++ it) {
        Entity* entity = it->second;
        
        entity->tick(tps);
    }
}


}

