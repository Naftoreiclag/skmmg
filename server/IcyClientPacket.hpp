#ifndef ICYCLIENTPACKET_HPP
#define ICYCLIENTPACKET_HPP

#include "IcyPacket.hpp"
#include "IcyProtocol.hpp"

namespace skm
{

class IcyClientPacket {
public:
    IcyClientPacket(IcyPacket* const data, IcyProtocol::SessionId& session);
    ~IcyClientPacket();
    
    IcyPacket* const data;
    const IcyProtocol::SessionId session;
};

}

#endif // ICYCLIENTPACKET_HPP
