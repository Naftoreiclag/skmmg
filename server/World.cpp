#include "World.hpp"

#include <iostream>
#include "IcyPacketPlayerJoin.hpp"

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
    m_server->m_outgoingGlobalPackets.push_back(new IcyPacketPlayerJoin(handle));
    
    std::cout << "Sent player" << std::endl;
}

}

