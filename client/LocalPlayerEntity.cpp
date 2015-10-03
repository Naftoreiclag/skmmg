#include "LocalPlayerEntity.hpp"

namespace skm
{

std::string LocalPlayerEntity::nextOgreName(){
    static unsigned long next = 0;
    
    std::stringstream ss;
    ss << "LocalPlayer";
    ss << next ++;
    return ss.str();
}

LocalPlayerEntity::LocalPlayerEntity(const Handle& handle, Ogre::SceneManager* smgr, IcyClient& client)
: Entity(handle, smgr)
, m_client(client)
, m_reconLoc(client) {
    m_ogreSceneNode = m_smgr->getRootSceneNode()->createChildSceneNode();
    m_ogreEntity = m_smgr->createEntity(nextOgreName(), "ogrehead.mesh");
    m_ogreSceneNode->attachObject(m_ogreEntity);
    m_ogreSceneNode->setScale(0.2f, 0.2f, 0.2f);
}

LocalPlayerEntity::~LocalPlayerEntity()
{
}
void LocalPlayerEntity::updateSceneNode() {
    if(m_smgr) {
        m_ogreSceneNode->setPosition(m_location.x, m_location.y, m_location.z);
    }
}

void LocalPlayerEntity::setLocation(Vec3f location) {
    m_reconLoc.authoritativeSet(location.x, location.z);
    m_location = location;
    updateSceneNode();
}

void LocalPlayerEntity::handlePacket(const IcyPacketReconciledLocationUpdate* packet) {
    m_reconLoc.handlePacket(packet);
    m_location.x = m_reconLoc.getX();
    m_location.z = m_reconLoc.getZ();
    updateSceneNode();
}

void LocalPlayerEntity::tick(float tpf) {
    
    m_reconLoc.tick();
    m_ogreSceneNode->rotate(Ogre::Vector3(0, 1, 0), Ogre::Radian(tpf));
}


}

