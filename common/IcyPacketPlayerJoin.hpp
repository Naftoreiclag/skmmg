#ifndef SKM_ICYPACKETPLAYERJOIN_HPP
#define SKM_ICYPACKETPLAYERJOIN_HPP

#include <string>
#include "IcyPacket.hpp"
#include "Entity.hpp"

namespace skm
{

// Sent to notify clients that a player has joined the game. Includes entity handle for that player
class IcyPacketPlayerJoin : public IcyPacket {
public:
    Entity::Handle m_handle;

    IcyPacketPlayerJoin();
    IcyPacketPlayerJoin(Entity::Handle handle);
    virtual ~IcyPacketPlayerJoin();
    
    virtual ProtocolId getId() const;
    virtual bool isContinuous() const;
    
    virtual bool read(sf::Packet& data);
    virtual void write(sf::Packet& data);
};

}

#endif // SKM_ICYPACKETPLAYERJOIN_HPP
