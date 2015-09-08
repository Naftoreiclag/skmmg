#ifndef WORLD_HPP
#define WORLD_HPP

#include <vector>

#include "Entity.hpp"
#include "Player.hpp"
#include "IcyServer.hpp"
#include "IcyProtocol.hpp"

namespace skm
{

class World
{
private:
    Entity::Handle m_lastHandle;
    Entity::Handle getNextHandle();
    
public:
    std::vector<Entity*> m_entities;
    std::vector<Player*> m_players;

    IcyServer* m_server;
    
    void spawnPlayer(const IcyProtocol::SessionId& sessionId);

    World();
    ~World();

};

}

#endif // WORLD_HPP
