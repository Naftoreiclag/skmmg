#include "IcyPacketHeartbeat.hpp"

namespace skm
{

IcyPacketHeartbeat::IcyPacketHeartbeat() {
}

IcyPacketHeartbeat::~IcyPacketHeartbeat() {
}

IcyPacket::ProtocolId IcyPacketHeartbeat::getId() const {
    return IcyPacket::s_protocol_heartbeat;
}
bool IcyPacketHeartbeat::isContinuous() const {
    return true;
}

bool IcyPacketHeartbeat::read(sf::Packet& data) {
    return true;
}
void IcyPacketHeartbeat::write(sf::Packet& data) {
    
}


}

