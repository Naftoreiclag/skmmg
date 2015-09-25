#ifndef ICYPROTOCOL_HPP
#define ICYPROTOCOL_HPP

#include "SFML/System.hpp"

namespace skm
{

namespace IcyProtocol
{
    typedef sf::Uint16 Port;
    
    typedef sf::Uint32 MagicNumber;
    typedef sf::Uint32 SequenceNumber;
    typedef sf::Uint64 AckBitfield;
    const SequenceNumber s_ackBitfieldSize = 64;
    
    typedef sf::Uint32 SessionId;

    const MagicNumber s_magicNumber = 0xDAFF0D11; // Magic number that make up the first bytes of every packet sent and received
    const MagicNumber s_magicHandshake = 0xD1AB011C; // Magic number used only for initial handshakes
    
    const sf::Int32 s_requestDelayMs = 1000; // Time between connection requests
    const sf::Int32 s_requestTimeoutMs = 15000; // If the server does not supply the session within this time, terminate connection
    const sf::Int32 s_verifyDelayMs = 1000; // Time between verification requests
    const sf::Int32 s_verifyTimeoutMs = 15000; // If the server does not verify the session within this time, terminate connection
    
    const sf::Int32 s_heartbeatDelayMs = 3000; // Time between sent heartbeats
    const sf::Int32 s_serverTimeoutMs = 15000; // If we do not receive any packets for this long, terminate connection
    
    const SessionId s_sessionRequestId = 0xFFFFFFFF;
}

}

#endif // ICYPROTOCOL_HPP
