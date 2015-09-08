#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "IcyProtocol.hpp"

#include "Entity.hpp"

namespace skm
{

class Player : public Entity
{
public:
    Player(const Entity::Handle& handle, const IcyProtocol::SessionId& sessionId);
    virtual ~Player();
    
    const IcyProtocol::SessionId m_sessionId;
};

}

#endif // PLAYER_HPP
