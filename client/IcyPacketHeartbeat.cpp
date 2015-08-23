#include "IcyPacketHeartbeat.hpp"

namespace skm
{

IcyPacketHeartbeat::IcyPacketHeartbeat() {
}

IcyPacketHeartbeat::~IcyPacketHeartbeat() {
}

ProtocolId IcyPacketHeartbeat::getId() const {
    return IcyPacket::s_protocol_heartbeat;
}
bool IcyPacketHeartbeat::isContinuous() const {
    return true;
}

bool IcyPacketHeartbeat::read(sf::Packet& data) {
    
}
void IcyPacketHeartbeat::write(sf::Packet& data) {
    
}


}

