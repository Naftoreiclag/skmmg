#include "IcyPacketEntitySpawn.hpp"

namespace skm
{

IcyPacketEntitySpawn::IcyPacketEntitySpawn() {
}
IcyPacketEntitySpawn::IcyPacketEntitySpawn(Entity::Handle handle)
: m_handle(handle) {
}

IcyPacketEntitySpawn::~IcyPacketEntitySpawn() {
}

IcyPacket::ProtocolId IcyPacketEntitySpawn::getId() const {
    return IcyPacket::s_protocol_entitySpawn;
}
bool IcyPacketEntitySpawn::isContinuous() const {
    return false;
}

bool IcyPacketEntitySpawn::read(sf::Packet& data) {
    data >> m_handle;
    return true;
}
void IcyPacketEntitySpawn::write(sf::Packet& data) {
    data << m_handle;
}


}

