#include "Player.hpp"

namespace skm
{

Player::Player(const Entity::Handle& handle, const IcyProtocol::SessionId& sessionId)
: Entity(handle)
, m_sessionId(sessionId) {
}

Player::~Player() {
}


}

