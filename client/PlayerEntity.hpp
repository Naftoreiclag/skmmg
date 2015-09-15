#ifndef SKM_PLAYERENTITY_HPP
#define SKM_PLAYERENTITY_HPP

#include "Entity.hpp"

#include "OgreSceneManager.h"
#include "OgreEntity.h"

namespace skm
{

class PlayerEntity : public Entity {
private:
    Vec3f m_location;
    
    Ogre::Entity* m_ogreEntity;
    Ogre::SceneNode* m_ogreSceneNode;
    
    static std::string nextOgreName();
    
public:
    PlayerEntity(const Handle& handle, Ogre::SceneManager* smgr);
    virtual ~PlayerEntity();
    virtual void setLocation(Vec3f location);
    virtual void tick(float tpf);

};

}

#endif // SKM_PLAYERENTITY_HPP
