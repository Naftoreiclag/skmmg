#include "IcyPacketChat.hpp"

namespace skm
{

IcyPacketChat::IcyPacketChat() {
}
IcyPacketChat::IcyPacketChat(std::string message)
: m_message(message) {
}

IcyPacketChat::~IcyPacketChat() {
}

IcyPacket::ProtocolId IcyPacketChat::getId() const {
    return IcyPacket::s_protocol_chat;
}
bool IcyPacketChat::isContinuous() const {
    return true;
}

bool IcyPacketChat::read(sf::Packet& data) {
    data >> m_message;
    return true;
}
void IcyPacketChat::write(sf::Packet& data) {
    data << m_message;
}


}

