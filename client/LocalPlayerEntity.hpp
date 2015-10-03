#ifndef SKM_LOCALPLAYERENTITY_HPP
#define SKM_LOCALPLAYERENTITY_HPP

#include "IcyClient.hpp"

#include "Entity.hpp"

#include "OgreSceneManager.h"
#include "OgreEntity.h"

#include "ReconciledLocation.hpp"

namespace skm {

class LocalPlayerEntity : public Entity {
private:
    Vec3f m_location;
    
    Ogre::Entity* m_ogreEntity;
    Ogre::SceneNode* m_ogreSceneNode;
    
    IcyClient& m_client;
    
    ReconciledLocation m_reconLoc;
    
    static std::string nextOgreName();
    
    void updateSceneNode();
public:
    LocalPlayerEntity(const Handle& handle, Ogre::SceneManager* smgr, IcyClient& client);
    ~LocalPlayerEntity();

public:
    virtual void setLocation(Vec3f location);
    virtual void tick(float tpf);
    
    void handlePacket(const IcyPacketReconciledLocationUpdate* packet);
};

}

#endif // SKM_LOCALPLAYERENTITY_HPP
