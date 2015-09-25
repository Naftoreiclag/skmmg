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
    
    typedef std::vector<Entity*> EntityContainer;
    typedef std::vector<Player*> PlayerContainer;
    
public:
    EntityContainer m_entities;
    PlayerContainer m_players;

    IcyServer* m_server;
    
    void spawnPlayer(const IcyProtocol::SessionId& sessionId);
    void despawnPlayer(const IcyProtocol::SessionId& sessionId);
    
    void spawnEntity();
    
    void tick(float tps);

    World();
    ~World();

};

}

#endif // WORLD_HPP
