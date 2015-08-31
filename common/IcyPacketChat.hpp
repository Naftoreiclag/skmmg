#ifndef ICYPACKETCHAT_HPP
#define ICYPACKETCHAT_HPP

#include <string>
#include "IcyPacket.hpp"

namespace skm
{

class IcyPacketChat : public IcyPacket {
public:
    
    std::string m_message;

    IcyPacketChat();
    IcyPacketChat(std::string m_message);
    ~IcyPacketChat();
    
    virtual ProtocolId getId() const;
    virtual bool isContinuous() const;
    
    virtual bool read(sf::Packet& data);
    virtual void write(sf::Packet& data);
};

}

#endif // ICYPACKETCHAT_HPP
