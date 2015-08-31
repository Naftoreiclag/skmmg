#include "IcyClientPacket.hpp"

namespace skm
{

IcyClientPacket::IcyClientPacket(IcyPacket* const data, IcyProtocol::SessionId& session)
: data(data)
, session(session) {
}

IcyClientPacket::~IcyClientPacket() {
}


}

