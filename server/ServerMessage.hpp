#ifndef SERVERMESSAGE_HPP
#define SERVERMESSAGE_HPP

namespace skm {

class ServerMessage {
public:
    enum Type {
        USER_JOIN,
        USER_LEAVE
    };
public:
    ServerMessage();
    virtual ~ServerMessage();
    
    virtual Type getType() const = 0;
};

}

#endif // SERVERMESSAGE_HPP
