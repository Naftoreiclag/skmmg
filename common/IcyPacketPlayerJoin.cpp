#include "IcyPacketPlayerJoin.hpp"

namespace skm
{

IcyPacketPlayerJoin::IcyPacketPlayerJoin() {
}
IcyPacketPlayerJoin::IcyPacketPlayerJoin(Entity::Handle handle)
: m_handle(handle) {
}

IcyPacketPlayerJoin::~IcyPacketPlayerJoin() {
}

IcyPacket::ProtocolId IcyPacketPlayerJoin::getId() const {
    return IcyPacket::s_protocol_playerJoin;
}
bool IcyPacketPlayerJoin::isContinuous() const {
    return false;
}

bool IcyPacketPlayerJoin::read(sf::Packet& data) {
    data >> m_handle;
    return true;
}
void IcyPacketPlayerJoin::write(sf::Packet& data) {
    data << m_handle;
}


}

