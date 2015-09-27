#include "IcyPacketReconciledLocationUpdate.hpp"

namespace skm
{
IcyPacketReconciledLocationUpdate::IcyPacketReconciledLocationUpdate() {
}
IcyPacketReconciledLocationUpdate::IcyPacketReconciledLocationUpdate(Sequence_t sequence, float x, float z)
: sequence(sequence)
, x(x)
, z(z) {
}

IcyPacketReconciledLocationUpdate::~IcyPacketReconciledLocationUpdate() {
}

IcyPacket::ProtocolId IcyPacketReconciledLocationUpdate::getId() const {
    return IcyPacket::s_protocol_reconciledLocationUpdate;
}
bool IcyPacketReconciledLocationUpdate::isContinuous() const {
    return false;
}

bool IcyPacketReconciledLocationUpdate::read(sf::Packet& data) {
    data << sequence;
    data << x;
    data << z;
    return true;
}
void IcyPacketReconciledLocationUpdate::write(sf::Packet& data) {
    data >> sequence;
    data >> x;
    data >> z;
}

}

