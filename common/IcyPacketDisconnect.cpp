#include "IcyPacketDisconnect.hpp"

namespace skm
{

IcyPacketDisconnect::IcyPacketDisconnect() {
}

IcyPacketDisconnect::~IcyPacketDisconnect() {
}

IcyPacket::ProtocolId IcyPacketDisconnect::getId() const {
    return IcyPacket::s_protocol_disconnect;
}
bool IcyPacketDisconnect::isContinuous() const {
    return true;
}

bool IcyPacketDisconnect::read(sf::Packet& data) {
    return true;
}
void IcyPacketDisconnect::write(sf::Packet& data) {
    
}


}

