#include "PlayerEntity.hpp"

#include <sstream>

namespace skm
{

std::string PlayerEntity::nextOgreName(){
    static unsigned long next = 0;
    
    std::stringstream ss;
    ss << "Player";
    ss << next;
    return ss.str();
}

PlayerEntity::PlayerEntity(const Handle& handle, Ogre::SceneManager* smgr)
: Entity(handle, smgr) {
    m_ogreSceneNode = m_smgr->getRootSceneNode()->createChildSceneNode();
    m_ogreEntity = m_smgr->createEntity(nextOgreName(), "ogrehead.mesh");
    m_ogreSceneNode->attachObject(m_ogreEntity);
}

PlayerEntity::~PlayerEntity()
{
}

void PlayerEntity::setLocation(Vec3f location) {
    m_location = location;
    if(m_smgr) {
        
    }
}

}

