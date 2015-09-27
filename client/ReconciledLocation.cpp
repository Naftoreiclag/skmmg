#include "ReconciledLocation.hpp"
#include <math.h>
#include <iostream>

namespace skm {

ReconciledLocation::Request::Request(Sequence_t sequence, float x, float z)
: sequence(sequence)
, x(x)
, z(z) {
}
    
ReconciledLocation::ReconciledLocation(IcyClient& client)
: m_client(client)
, m_x(0)
, m_z(0)
, m_idle(true)
, m_sequence(0)
, m_conseqReqSent(0) {
}

ReconciledLocation::~ReconciledLocation()
{
}

// Request that the x and z values change to
void ReconciledLocation::requestSet(float x, float z) {
    m_x = x;
    m_z = z;
    
    if(m_idle) {
        sendRequestNow();
    }
    m_idle = false;
}

// Handle packet
void ReconciledLocation::handlePacket(IcyPacketReconciledLocationUpdate* packet) {
    
    // When the update has been found
    bool found = false;
    
    // If the response is within range or not
    bool correct = false;
    std::vector<Request>::iterator delSpot;
    
    for(std::vector<Request>::iterator it = m_requests.begin(); it != m_requests.end(); ++ it) {
        Request& req = *it;
        
        if(req.sequence == packet->sequence) {
            float dx = packet->x - req.x;
            float dz = packet->z - req.z;
            if(dx < 0) dx = -dx;
            if(dz < 0) dz = -dz;
            
            found = true;
            correct = dx < s_tolerance && dz < s_tolerance;
            
            // The response is within range of the request
            if(correct) {
                delSpot = it;
                break;
            }
        }
    }
    
    // Found the corresponding request
    if(found) {
        // The response is what we wanted
        if(correct) {
            // Erase the request and all earlier requests
            ++ delSpot;
            m_requests.erase(m_requests.begin(), delSpot);
        }
        
        // The response is a totally different location than expected
        else {
            // Teleport to new location
            
            if(std::isnan(packet->z) || std::isnan(packet->x)) {
                std::cout << "nan!" << std::endl;
            }
            m_x = packet->x;
            m_z = packet->z;
        }
    }
    
    // Not found???
    else {
        
    }
}
float ReconciledLocation::getX() const {
    return m_x;
}
float ReconciledLocation::getZ() const {
    return m_z;
}



// Hint that there will be a long break in the requests, so queue and send a final request immediately
void ReconciledLocation::hintEndOfRequests() {
    if(!m_idle && m_conseqReqSent >= s_minQuantityBeforeHint) {
        sendRequestNow();
        m_idle = true;
    }
}

void ReconciledLocation::tick() {
    // Moving around
    if(!m_idle) {
        if(m_sendTimer.getElapsedTime().asMilliseconds() >= IcyPacketReconciledLocationUpdate::s_requestIntervalMs) {
            // If this request would be exactly the same as the one we sent earlier
            if(m_xReq == m_x && m_zReq == m_z) {
                // Then we are now idle
                m_conseqReqSent = 0;
                m_idle = true;
            }
            
            // The request would be different
            else {
                sendRequestNow();
                
                // Reset request timer
                m_sendTimer.restart();
            }
        }
    }
    
    // Doing nothing?
    else {
        // Keep doing nothing
    }
}
void ReconciledLocation::sendRequestNow() {
    // Send request
    m_client.m_outgoingPackets.push_back(new IcyPacketReconciledLocationUpdate(m_sequence, m_x, m_z));
    m_requests.push_back(Request(m_sequence, m_x, m_z));
    
    // Update sequence
    ++ m_sequence;
    
    // Update record of what we sent last
    m_xReq = m_x;
    m_xReq = m_z;
    
    // Count how many consequtive requests have been sent
    ++ m_conseqReqSent;
}

}

