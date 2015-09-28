#include "IcyPacketReconciledLocationUpdate.hpp"

#include <iostream>

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
    if(std::isnan(z) || std::isnan(x)) {
        std::cout << "Packet is nan!" << std::endl;
        return false;
    }
    return true;
}
void IcyPacketReconciledLocationUpdate::write(sf::Packet& data) {
    data >> sequence;
    data >> x;
    data >> z;
}

}

