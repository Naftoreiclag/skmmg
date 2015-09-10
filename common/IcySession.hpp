#ifndef ICYSESSION_HPP
#define ICYSESSION_HPP

#include <mutex>
#include <list>

#include "SFML/Network.hpp"

#include "IcyPacket.hpp"
#include "ThreadQueue.hpp"
#include "IcyProtocol.hpp"

namespace skm
{

class IcySession
{
public:
    IcyProtocol::SessionId m_sessionId;
    sf::UdpSocket* m_socket;
    
    sf::IpAddress m_serverAddress;
    IcyProtocol::Port m_serverPort;
    
    // The sequence number to use when sending a new packet
    IcyProtocol::SequenceNumber m_localSequence;
    
    // The most recent packet that we have receieved
    bool m_neverReceievedPacketBefore;
    IcyProtocol::SequenceNumber m_ack;
    IcyProtocol::AckBitfield m_ackBits;
    
    struct SentPacket {
        IcyProtocol::SequenceNumber sequence;
        IcyPacket* data;
    };

    std::list<SentPacket> m_sentPackets;
public:
    IcySession();
    ~IcySession();
    
    // Add packets to this queue to have them sent to the remote; IcySession will handle their deletion
    ThreadQueue<IcyPacket*>* m_outgoingPackets;
    
    IcyPacket* processRawIncoming(sf::Packet& packet);
    
    // This should only be called while iterating through m_outgoingPackets
    void sendOutgoing(IcyPacket* packet);
};

}

#endif // ICYSESSION_HPP
