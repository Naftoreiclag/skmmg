#ifndef SERVERMSG_HPP
#define SERVERMSG_HPP

#include "IcyProtocol.hpp"

namespace skm {

class ServerMsg {
public:
    enum Type {
        USER_JOIN,
        USER_LEAVE
    };
public:
    ServerMsg();
    ServerMsg(Type type, IcyProtocol::SessionId session);
    ~ServerMsg();
    
    Type m_type;
    
    IcyProtocol::SessionId m_session;
};

}

#endif // SERVERMSG_HPP
