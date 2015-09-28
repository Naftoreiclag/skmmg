#include "IcyPacketSquareRoot.hpp"

namespace skm
{

IcyPacketSquareRoot::IcyPacketSquareRoot() {
}

IcyPacketSquareRoot::~IcyPacketSquareRoot() {
}

IcyPacket::ProtocolId IcyPacketSquareRoot::getId() const {
    return IcyPacket::s_protocol_squareRoot;
}
bool IcyPacketSquareRoot::isContinuous() const {
    return false;
}

bool IcyPacketSquareRoot::read(sf::Packet& data) {
    data >> danger;
    
    return true;
}
void IcyPacketSquareRoot::write(sf::Packet& data) {
    data << danger;
    
}


}

