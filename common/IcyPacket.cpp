#include "IcyPacket.hpp"

#include "IcyPacketHeartbeat.hpp"
#include "IcyPacketChat.hpp"

namespace skm
{
    
IcyPacket* IcyPacket::newPacketFromRaw(sf::Packet& packet) {
    ProtocolId pid;
    packet >> pid;
    
    if(pid == s_protocol_heartbeat) {
        return new IcyPacketHeartbeat();
    }
    else if(pid == s_protocol_chat) {
        IcyPacket* p = new IcyPacketChat();
        p->read(packet);
        return p;
    }
    
    return nullptr;
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

