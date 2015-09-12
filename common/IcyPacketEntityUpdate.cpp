#include "IcyPacketEntityUpdate.hpp"

namespace skm
{

IcyPacketEntityUpdate::IcyPacketEntityUpdate() {
}
IcyPacketEntityUpdate::IcyPacketEntityUpdate(Entity::Handle handle)
: m_handle(handle) {
}

IcyPacketEntityUpdate::~IcyPacketEntityUpdate() {
}

IcyPacket::ProtocolId IcyPacketEntityUpdate::getId() const {
    return IcyPacket::s_protocol_entityUpdate;
}
bool IcyPacketEntityUpdate::isContinuous() const {
    return false;
}

bool IcyPacketEntityUpdate::read(sf::Packet& data) {
    data >> m_handle;
    return true;
}
void IcyPacketEntityUpdate::write(sf::Packet& data) {
    data << m_handle;
}

void IcyPacketEntityUpdate::despawn() {
    exists = false;
}


}

