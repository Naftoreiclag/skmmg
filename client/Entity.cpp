#include "Entity.hpp"

namespace skm {

Entity::Entity(const Handle& handle, Ogre::SceneManager* smgr)
: m_handle(handle)
, m_smgr(smgr) {
}

Entity::~Entity() {
}

}

