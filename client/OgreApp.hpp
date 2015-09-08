#ifndef OGREAPP_HPP
#define OGREAPP_HPP

#include "OgreRoot.h"
#include "OgreCamera.h"
#include "OgreSceneManager.h"
#include "OgreRenderWindow.h"

class OgreApp {
public:
    OgreApp();
    virtual ~OgreApp();
    void run();
protected:
    Ogre::Root* m_ogreRoot;
    Ogre::Camera* m_cam;
    Ogre::SceneManager* m_smgr;
    Ogre::RenderWindow* m_window;
};

#endif // OGREAPP_HPP
