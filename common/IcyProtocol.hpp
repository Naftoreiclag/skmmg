#ifndef ICYPROTOCOL_HPP
#define ICYPROTOCOL_HPP

namespace skm
{

namespace IcyProtocol
{
    typedef sf::Uint16 Port;
    
    typedef sf::Uint32 MagicNumber;
    typedef sf::Uint32 SequenceNumber;
    typedef sf::Uint64 AckBitfield;
    static const SequenceNumber s_ackBitfieldSize = 64;
    
    typedef sf::Uint32 SessionId;

    static const MagicNumber s_magicNumber = 0xDAFF0D11; // Magic number that make up the first bytes of every packet sent and received
    static const MagicNumber s_magicHandshake = 0xD1AB011C; // Magic number used only for initial handshakes
    
    static const sf::Int32 s_requestDelayMs = 750; // Time between connection requests
    static const sf::Int32 s_requestTimeoutMs = 15000; // If the server does not supply the session within this time, terminate connection
    static const sf::Int32 s_verifyDelayMs = 750; // Time between verification requests
    static const sf::Int32 s_verifyTimeoutMs = 15000; // If the server does not verify the session within this time, terminate connection
    
    static const sf::Int32 s_heartbeatDelayMs = 500; // Time between sent heartbeats
    static const sf::Int32 s_serverTimeoutMs = 15000; // If we do not receive any packets for this long, terminate connection
    
    static const SessionId s_sessionRequestId = 0xFFFFFFFF;
}

}

#endif // ICYPROTOCOL_HPP
