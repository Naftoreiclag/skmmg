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
    data >> m_exists;
    data >> m_changeLoc;
    data >> m_loc;
    return true;
}
void IcyPacketEntityUpdate::write(sf::Packet& data) {
    data << m_handle;
    data << m_exists;
    data << m_changeLoc;
    data << m_loc;
}

void IcyPacketEntityUpdate::despawn() {
    m_exists = false;
}

void IcyPacketEntityUpdate::setLoc(Vec3f& loc) {
    m_changeLoc = true;
    m_loc = loc;
}


}

