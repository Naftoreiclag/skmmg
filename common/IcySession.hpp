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
    struct Status {
        Status();
        
        bool serverContacted;
        bool sessionVerified;
        bool connected;
    };
    
public:
    IcyProtocol::SessionId m_sessionId;
    sf::UdpSocket* m_socket;
    
    sf::IpAddress m_serverAddress;
    IcyProtocol::Port m_serverPort;
    
    Status m_status;
    std::mutex m_status_mutex;
    
    IcyProtocol::SequenceNumber m_localSequence;
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
