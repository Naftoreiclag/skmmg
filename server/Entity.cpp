#include "Entity.hpp"

namespace skm
{

Entity::Entity(const Handle& handle)
: m_handle(handle)
, m_updatePacket(nullptr) {
}

Entity::~Entity() {
}

void Entity::setLocation(Vec3f location) {
    m_location = location;
}

}

