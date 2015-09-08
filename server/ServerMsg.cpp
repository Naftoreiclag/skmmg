#include "ServerMsg.hpp"

namespace skm
{

ServerMsg::ServerMsg() {
    
}
    
ServerMsg::ServerMsg(Type type, IcyProtocol::SessionId session)
: m_type(type)
, m_session(session) {}

ServerMsg::~ServerMsg() {}


}

