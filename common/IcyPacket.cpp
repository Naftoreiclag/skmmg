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

IcyPacket::IcyPacket() {
}

IcyPacket::~IcyPacket() {
}


}

