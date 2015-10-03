#include "World.hpp"

#include <iostream>
#include "IcyPacketPlayerJoin.hpp"
#include "IcyPacketEntitySpawn.hpp"
#include "IcyPacketEntityUpdate.hpp"

namespace skm
{

World::World()
: m_lastHandle(555) {
}

World::~World()
{
}

Entity::Handle World::getNextHandle() {
    return m_lastHandle ++;
}

void World::spawnPlayer(const IcyProtocol::SessionId& sessionId) {
    Entity::Handle handle = getNextHandle();
    Player* player = new Player(handle, sessionId);
    
    m_players.push_back(player);
    m_server->send(new IcyPacketEntitySpawn(handle));
    m_server->send(new IcyPacketPlayerJoin(handle), sessionId);
}
void World::despawnPlayer(const IcyProtocol::SessionId& sessionId) {
    
    PlayerContainer::iterator it;
    Player* player;
    bool found = false;
    for(it = m_players.begin(); it != m_players.end(); ++ it) {
        player = *it;
        
        if(player->m_sessionId == sessionId) {
            found = true;
            break;
        }
    }
    
    if(found) {
        m_players.erase(it);
        
        IcyPacketEntityUpdate* update = new IcyPacketEntityUpdate(player->m_sessionId);
        update->despawn();
        
        m_server->send(update);
        
        delete player;
    }
    
}
Player* World::getPlayer(const IcyProtocol::SessionId& sessionId) const {
    for(PlayerContainer::const_iterator it = m_players.begin(); it != m_players.end(); ++ it) {
        Player* player = *it;
        
        if(player->m_sessionId == sessionId) {
            return player;
        }
    }
    
    return nullptr;
}


void World::tick(float tps) {
    for(PlayerContainer::const_iterator it = m_players.begin(); it != m_players.end(); ++ it) {
        Player* player = *it;
        
        if(player->m_updatePacket) {
            m_server->send(player->m_updatePacket);
            player->m_updatePacket = nullptr;
        }
    }
    
}

}

