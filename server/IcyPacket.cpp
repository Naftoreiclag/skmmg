#include "IcyPacket.hpp"
#include "IcyPacketHeartbeat.hpp"

namespace skm
{
    
IcyPacket* IcyPacket::newPacketFromRaw(sf::Packet& packet) {
    ProtocolId pid;
    packet >> pid;
    
    if(pid == s_protocol_heartbeat) {
        return new IcyPacketHeartbeat();
    }
    
    return nullptr;
}

IcyPacket::IcyPacket() {
}

IcyPacket::~IcyPacket() {
}


}

