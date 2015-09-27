#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "IcyProtocol.hpp"
#include "IcyPacketReconciledLocationUpdate.hpp"

#include "Entity.hpp"

namespace skm
{

class Player : public Entity
{
public:
    Player(const Entity::Handle& handle, const IcyProtocol::SessionId& sessionId);
    virtual ~Player();
    
    const IcyProtocol::SessionId m_sessionId;
    
    IcyPacketReconciledLocationUpdate::Sequence_t m_reconLocSeq;
    IcyPacketReconciledLocationUpdate* handlePacket(const IcyPacketReconciledLocationUpdate* packet);
};

}

#endif // PLAYER_HPP
