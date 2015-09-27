#include "Player.hpp"

#include <iostream>

namespace skm
{

Player::Player(const Entity::Handle& handle, const IcyProtocol::SessionId& sessionId)
: Entity(handle)
, m_sessionId(sessionId)
, m_reconLocSeq(0) {
}

Player::~Player() {
}


IcyPacketReconciledLocationUpdate* Player::handlePacket(const IcyPacketReconciledLocationUpdate* packet) {
    
    float dx = packet->x - m_location.x;
    float dz = packet->z - m_location.z;
    
    // Placeholder range checking
    if(dx > 5 || dx < 5 || dz > 5 || dz < 5) {
        // ???
    }
    
    m_location.x = packet->x;
    m_location.z = packet->z;
    
    if(m_location.x > 30) {
        m_location.x = 0;
        
        std::cout << "range" << std::endl;
    }
    
    m_reconLocSeq = packet->sequence;
    
    return new IcyPacketReconciledLocationUpdate(m_reconLocSeq, m_location.x, m_location.z);
}

}

