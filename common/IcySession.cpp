#include "IcySession.hpp"

#include "DebugAwareness.hpp"

#ifndef NICYDEBUG
#include <iostream>
#endif
#include <iostream>

namespace skm
{

IcySession::IcySession()
: m_localSequence(0)
, m_ack(0)
, m_ackBits(0)
, m_neverReceievedPacketBefore(true) {
}

IcySession::~IcySession() {}

IcyPacket* IcySession::processRawIncoming(sf::Packet& packet) {
    // Update remote sequence number and ack field
    {
        // Get when this packet was sent by the server
        IcyProtocol::SequenceNumber remoteSeq;
        packet >> remoteSeq;
        
        // Never receieved a packet before, obviously this packet should be processed
        if(m_neverReceievedPacketBefore) {
            m_ack = remoteSeq;
            m_neverReceievedPacketBefore = false;
        }
        
        // This packet is more recent than the previous most recent packet
        else if(remoteSeq > m_ack) {
            // Find how much the ack bitfield should be shifted
            /*
             * Example:
             *      Received: 9
             *      Most up-to-date: 5
             * 
             *      Position: 9 - 5 = 4
             * 
             *         0b00000000      ...     9
             *           76543210   Bit position
             *          (    1234   Acks represented before shift)
             *          (12345678   New acks represented after shift)
             * 
             */
            
            IcyProtocol::SequenceNumber shift = remoteSeq - m_ack;
            
            // Shift the bits, setting bit -1 to be 1 (This way, the ack represented by the full 64 bit number is properly remembered)
            m_ackBits <<= 1;
            m_ackBits |= 1;
            m_ackBits <<= shift - 1;
            
            // This packet is now the most recent packet
            m_ack = remoteSeq;
        }
        
        // This packet was supposed to be receieved earlier
        else if(remoteSeq < m_ack) {
            // Get where this is stored in the ack bitfield
            /*
             * Example:
             *      Received: 3
             *      Most up-to-date: 8
             * 
             *      Position: (8 - 3) - 1 = 4
             * 
             *              v Least significant bit is one packet before the most recent
             *         0b00000000      ...      8
             *           76543210   Bit position
             *          (01234567   Acks represented)
             */
            IcyProtocol::SequenceNumber bitPos = (m_ack - remoteSeq) - 1;
            
            // This packet has not been receieved before
            if((m_ackBits & (1 << bitPos)) == 0) {
                
                // Remember that we have now receieved it
                m_ackBits |= 1 << bitPos;
            }
            
            // We have already receieved this exact packet because the sequence number was set in the bitfield, so do nothing
            else {
                return nullptr;
            }
        }
        
        // We have already receieved this exact packet because the sequence number is the same, so do nothing
        else {
            return nullptr;
        }
    }
    
    // Get acks from the server and queue resending of lost packets
    {
        IcyProtocol::SequenceNumber firstAck;
        IcyProtocol::AckBitfield prevAcks;
        packet >> firstAck;
        packet >> prevAcks;
        
        std::list<SentPacket>::iterator it = m_sentPackets.begin();
        while(it != m_sentPackets.end()) {
            SentPacket& sentPacket = *it;
            
            // This is the packet referenced by the fully referenced ack
            if(sentPacket.sequence == firstAck) {
                // Delete the packet data (since there is no need to ever resend it)
                sentPacket.data->drop();
                
                // Remove this packet from the list
                it = m_sentPackets.erase(it);
                continue;
            }
            
            // Packet is in the remote computer's future, and therefore could not have been acked
            if(sentPacket.sequence >= firstAck) {
                ++ it;
                continue;
            }
            
            // Get packet age (position in bitfield)
            // Careful of negative numbers here! IcyProtocol::SequenceNumber is unsigned!
            IcyProtocol::SequenceNumber bitPos = (firstAck - sentPacket.sequence) - 1;
            
            // Packet is within the last [ackBitfieldSize] sent
            if(bitPos < IcyProtocol::s_ackBitfieldSize) {
                // Packet is acked in the bitfield
                if((prevAcks & (1 << bitPos)) != 0) {
                    // Delete the packet data (since there is no need to ever resend it)
                    sentPacket.data->drop();
                
                    // Remove this packet from the list
                    it = m_sentPackets.erase(it);
                    continue;
                }
            }
            
            // Packet is too old to be in the bitfield
            else {
                // Resend packet with new id
                sentPacket.data->dropNoDelete();
                m_outgoingPackets->push_back(sentPacket.data);
                
                // Remove this packet from the list
                it = m_sentPackets.erase(it);
                continue;
            }
            
            // Packet is not yet acked, but might be in the future
            ++ it;
        }
    }
    
    // Finally, we can process the actual data
    {
        IcyPacket* newPacket = IcyPacket::newPacketFromRaw(packet);
        
        if(newPacket == nullptr) {
            return nullptr;
        }
        
        // This packet is a heartbeat
        if(newPacket->getId() == IcyPacket::s_protocol_heartbeat) {
            // Since heartbeats are only sent in the absense of data, just delete it
            delete newPacket;
            return nullptr;
        }
        
        // This packet is not a heartbeat; someone else will need it
        else {
            return newPacket;
        }
    }
}

void IcySession::sendOutgoing(IcyPacket* packet) {
    sf::Packet rawPacket;
    
    // Magic number
    rawPacket << IcyProtocol::s_magicNumber;
    
    // Local sequence
    rawPacket << m_localSequence;
    
    // Ack
    rawPacket << m_ack;
    
    // Ack bitfield
    rawPacket << m_ackBits;
    
    // Actual data
    rawPacket << packet->getId();
    
    packet->write(rawPacket);
    
    m_socket->send(rawPacket, m_serverAddress, m_serverPort);
    
    // Remove unverified continuous packets
    if(packet->isContinuous()) {
        std::list<SentPacket>::iterator it = m_sentPackets.begin();
        while(it != m_sentPackets.end()) {
            SentPacket& sentPacket = *it;
            
            // The previous packet should not be resent
            if(sentPacket.data->getId() == packet->getId()) {
                // Delete the packet data (since there is no need to ever resend it)
                sentPacket.data->drop();
            
                // Remove this packet from the list
                it = m_sentPackets.erase(it);
                continue;
            }
            
            // This packet is not relevant
            ++ it;
        }
    }
    
    SentPacket metadata;
    metadata.data = packet;
    metadata.sequence = m_localSequence;
    m_sentPackets.push_back(metadata);
    packet->grab();
    
    ++ m_localSequence;
}

}

