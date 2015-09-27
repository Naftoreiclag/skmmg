#ifndef SKM_ICYPACKETRECONCILEDLOCATIONUPDATE_HPP
#define SKM_ICYPACKETRECONCILEDLOCATIONUPDATE_HPP

/*
 * When serverbound:
 *      Requested location
 * 
 * When clientbound:
 *      Authoritative location
 * 
 */

#include "SFML/System.hpp"

#include "IcyPacket.hpp"

namespace skm
{

class IcyPacketReconciledLocationUpdate : public IcyPacket {
public:

    static const sf::Int32 s_requestIntervalMs = 100;

    float x;
    float z;
    
    typedef sf::Uint32 Sequence_t;
    
    Sequence_t sequence;

    IcyPacketReconciledLocationUpdate();
    IcyPacketReconciledLocationUpdate(Sequence_t sequence, float x, float z);
    virtual ~IcyPacketReconciledLocationUpdate();
    
    virtual ProtocolId getId() const;
    virtual bool isContinuous() const;
    
    virtual bool read(sf::Packet& data);
    virtual void write(sf::Packet& data);
};

}

#endif // SKM_ICYPACKETRECONCILEDLOCATIONUPDATE_HPP
