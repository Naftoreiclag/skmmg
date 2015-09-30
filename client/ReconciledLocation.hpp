#ifndef SKM_RECONCILEDLOCATION_HPP
#define SKM_RECONCILEDLOCATION_HPP

#include <vector>

#include "SFML/System.hpp"

#include "IcyClient.hpp"
#include "IcyPacketReconciledLocationUpdate.hpp"

namespace skm
{

class ReconciledLocation {
public:
    
    typedef IcyPacketReconciledLocationUpdate::Sequence_t Sequence_t;
private:

    IcyClient& m_client;
    
    
    unsigned int m_conseqReqSent;
    
    bool m_idle;
    Sequence_t m_sequence;
    Sequence_t m_ignoreLessThan; // Ignore position updates with a sequence number less than this value
    
    float m_xReq;
    float m_zReq;
    
    float m_x;
    float m_z;
    
    constexpr static float s_tolerance = 0.01f;
    
    struct Request {
        Request(Sequence_t sequence, float x, float z);
        
        Sequence_t sequence;
        float x;
        float z;
    };
    
    sf::Clock m_sendTimer;
    std::vector<Request> m_requests;
    
    void sendRequestNow();
    
public:
    // The minimum number of normally timed requests to be sent before we honor an EndOfRequests hint.
    // This is to try avoid spam caused by overcalling hintEndOfRequests()
    static const sf::Uint32 s_minQuantityBeforeHint = 4;

    ReconciledLocation(IcyClient& client);
    ~ReconciledLocation();
    
    // Hint that there will be a long break in the requests, so queue all movement and send a final request immediately
    void hintEndOfRequests();
    
    // Request that the x and z values change to
    void requestSet(float x, float z);
    
    float getX() const;
    float getZ() const;
    
    void handlePacket(IcyPacketReconciledLocationUpdate* packet);
    
    void tick();
};

}

#endif // SKM_RECONCILEDLOCATION_HPP
