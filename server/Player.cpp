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
    
    Vec3f newLoc = m_location;
    newLoc.x = packet->x;
    newLoc.z = packet->z;
    
    // Placeholder physics
    if(newLoc.x > 30) {
        newLoc.x = 30;
        
        std::cout << "range" << std::endl;
    }
    
    if(newLoc != m_location) {
        if(!m_updatePacket) {
            m_updatePacket = new IcyPacketEntityUpdate();
        }
        m_updatePacket->setLoc(m_location);
    }
    
    m_reconLocSeq = packet->sequence;
    return new IcyPacketReconciledLocationUpdate(m_reconLocSeq, m_location.x, m_location.z);
}

}

