#ifndef OGREAPP_HPP
#define OGREAPP_HPP

#include "IcyClient.hpp"

#include "OgreRoot.h"
#include "OgreCamera.h"
#include "OgreSceneManager.h"
#include "OgreRenderWindow.h"

namespace skm {

class OgreApp {
public:
    OgreApp(IcyClient& client);
    virtual ~OgreApp();
    void run();
private:
    IcyClient& m_client;

    Ogre::Root* m_ogreRoot;
    Ogre::Camera* m_cam;
    Ogre::SceneManager* m_smgr;
    Ogre::RenderWindow* m_window;
};

}

#endif // OGREAPP_HPP
