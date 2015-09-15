#ifndef ENTITY_HPP
#define ENTITY_HPP

#include "SFML/System.hpp"
#include "OgreSceneManager.h"

#include "Vec3f.hpp"
#include "EntityProtocol.hpp"

namespace skm {

class Entity {
public:
    typedef EntityProtocol::Handle Handle;
public:
    const Handle m_handle;
    Ogre::SceneManager* const m_smgr;

    Entity(const Handle& handle, Ogre::SceneManager* smgr);
    virtual ~Entity();
    
    virtual void setLocation(Vec3f location) = 0;
    virtual void tick(float tpf) = 0;
};

}

#endif // ENTITY_HPP
