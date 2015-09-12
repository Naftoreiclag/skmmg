#include "IcyPacket.hpp"

#include "IcyPacketHeartbeat.hpp"
#include "IcyPacketChat.hpp"
#include "IcyPacketPlayerJoin.hpp"
#include "IcyPacketDisconnect.hpp"
#include "IcyPacketEntitySpawn.hpp"
#include "IcyPacketEntityUpdate.hpp"

namespace skm
{
    
IcyPacket* IcyPacket::newPacketFromRaw(sf::Packet& packet) {
    ProtocolId pid;
    packet >> pid;
    
    IcyPacket* p;
    switch(pid) {
        case s_protocol_heartbeat: {
            p = new IcyPacketHeartbeat();
            break;
        }
        case s_protocol_disconnect: {
            p = new IcyPacketDisconnect();
            break;
        }
        case s_protocol_chat: {
            p = new IcyPacketChat();
            break;
        }
        case s_protocol_playerJoin: {
            p = new IcyPacketPlayerJoin();
            break;
        }
        case s_protocol_entitySpawn: {
            p = new IcyPacketEntitySpawn();
            break;
        }
        case s_protocol_entityUpdate: {
            p = new IcyPacketEntityUpdate();
            break;
        }
        default: {
            p = nullptr;
            break;
        }
    }
    
    if(p != nullptr) {
        p->read(packet);
    }
    
    return p;
}

void IcyPacket::grab() {
    ++ m_numGrabbers;
}

void IcyPacket::grab(sf::Uint32 numGrabbers) {
    m_numGrabbers += numGrabbers;
}

void IcyPacket::drop() {
    if(m_numGrabbers == 0) {
        delete this;
        return;
    }
    else {
        -- m_numGrabbers;
        if(m_numGrabbers == 0) {
            delete this;
            return;
        }
    }
}

void IcyPacket::dropNoDelete() {
    if(m_numGrabbers == 0) {
        return;
    }
    else {
        -- m_numGrabbers;
        if(m_numGrabbers == 0) {
            return;
        }
    }
}

IcyPacket::IcyPacket()
: m_numGrabbers(0) {
}

IcyPacket::~IcyPacket() {
}


}

