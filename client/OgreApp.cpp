#include "OgreApp.hpp"

#include <iostream>
#include <math.h>

#include "OgreLogManager.h"
#include "OgreViewport.h"
#include "OgreConfigFile.h"
#include "OgreEntity.h"
#include "OgreWindowEventUtilities.h"

#include "SFML/Window.hpp"

#include "IcyClient.hpp"
#include "IcySession.hpp"
#include "IcyPacketChat.hpp"
#include "IcyPacketPlayerJoin.hpp"
#include "IcyPacketEntitySpawn.hpp"
#include "IcyPacketEntityUpdate.hpp"
#include "IcyPacketSquareRoot.hpp"

#include "World.hpp"

#include "ReconciledLocation.hpp"

#include "KeyConfig.hpp"

namespace skm {
    
OgreApp::OgreApp(IcyClient& client)
: m_client(client)
, m_ogreRoot(nullptr)
, m_cam(nullptr)
, m_smgr(nullptr)
, m_window(nullptr) {
}

OgreApp::~OgreApp() {
    if(m_ogreRoot) delete m_ogreRoot;
}

void OgreApp::run() {
    m_ogreRoot = new Ogre::Root("plugins.cfg");
    
    // Load resource config
    {
        Ogre::ConfigFile resCfg;
        resCfg.load("resources.cfg");
        Ogre::ConfigFile::SectionIterator sectionIterator = resCfg.getSectionIterator();
        
        while(sectionIterator.hasMoreElements()) {
            Ogre::String sectionName = sectionIterator.peekNextKey();
            Ogre::ConfigFile::SettingsMultiMap* sectionData = sectionIterator.getNext();
            Ogre::ConfigFile::SettingsMultiMap::iterator dataIterator;
            for(dataIterator = sectionData->begin(); dataIterator != sectionData->end(); ++ dataIterator) {
                Ogre::ResourceGroupManager::getSingleton().addResourceLocation(dataIterator->second, dataIterator->first, sectionName);
            }
        }
    }
    
    if(m_ogreRoot->restoreConfig() || m_ogreRoot->showConfigDialog()) {
        m_ogreRoot->initialise(false, "OgreApp");
        m_window = m_ogreRoot->createRenderWindow("Name", 1280, 720, false);
    }
    else {
        return;
    }
    
    m_smgr = m_ogreRoot->createSceneManager(Ogre::ST_GENERIC);
    
    m_cam = m_smgr->createCamera("Camera");
    
    m_cam->setPosition(Ogre::Vector3(0, 0, 80));
    m_cam->lookAt(Ogre::Vector3(0, 0, -300));
    m_cam->setNearClipDistance(5);
    
    Ogre::Viewport* viewport = m_window->addViewport(m_cam);
    viewport->setBackgroundColour(Ogre::ColourValue(0, 0, 0));
    
    m_cam->setAspectRatio(Ogre::Real(viewport->getActualWidth()) / Ogre::Real(viewport->getActualHeight()));
    
    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
    
    Ogre::SceneNode* headNode = m_smgr->getRootSceneNode()->createChildSceneNode();
    Ogre::Entity* ogreHead = m_smgr->createEntity("Head", "ogrehead.mesh");
    headNode->attachObject(ogreHead);
    headNode->setScale(0.2f, 0.2f, 0.2f);
    
    m_smgr->setAmbientLight(Ogre::ColourValue(0.5, 0.5, 0.5));
    
    m_smgr->setSkyBox(true, "Test");
    
    Ogre::Light* light = m_smgr->createLight("Light");
    light->setPosition(20,80,50);
    
    World world(m_client, m_smgr);
    
    sf::Clock oneSecond;
    unsigned int numFrames = 0;
    unsigned int numPrints = 0;
    unsigned int totalFrames = 0;
    
    m_client.prepareTimeouts();
    sf::Clock tpsTimer;
    while(true) {
        float tps = tpsTimer.getElapsedTime().asSeconds();
        tpsTimer.restart();
        
        if(oneSecond.getElapsedTime().asMilliseconds() >= 1000) {
            std::cout << "FPS: " << numFrames;
            ++ numPrints;
            totalFrames += numFrames;
            numFrames = 0;
            std::cout << "\tAverage: " << (totalFrames / numPrints) << std::endl;
            oneSecond.restart();
        }
        else {
            ++ numFrames;
        }
        
        m_client.sustainConnection();
        
        //reconLoc.tick();
        world.tick(tps);
        
        
        IcyPacket* data;
        bool dataPopped = m_client.m_incomingPackets.pop_front(data);
        while(dataPopped) {
            
            switch(data->getId()) {
                case IcyPacket::s_protocol_chat: {
                    IcyPacketChat* chatPack = (IcyPacketChat*) data;
                    
                    std::cout << "null" << ":" << chatPack->m_message << std::endl;
                    break;
                }
                case IcyPacket::s_protocol_playerJoin: {
                    IcyPacketPlayerJoin* playerJoined = (IcyPacketPlayerJoin*) data;
                    world.playerJoin(playerJoined);
                    break;
                }
                case IcyPacket::s_protocol_entitySpawn: {
                    IcyPacketEntitySpawn* entSpawn = (IcyPacketEntitySpawn*) data;
                    world.spawnEntity(*entSpawn);
                    break;
                }
                case IcyPacket::s_protocol_entityUpdate: {
                    IcyPacketEntityUpdate* entUpdate = (IcyPacketEntityUpdate*) data;
                    world.updateEntity(*entUpdate);
                    break;
                }
                case IcyPacket::s_protocol_reconciledLocationUpdate: {
                    IcyPacketReconciledLocationUpdate* upd = (IcyPacketReconciledLocationUpdate*) data;
                    world.reconLocUpdate(upd);
                    //reconLoc.handlePacket(reconLocUpdate);
                    break;
                }
                default: {
                    break;
                }
            }
            
            delete data;
            dataPopped = m_client.m_incomingPackets.pop_front(data);
        }
        IcyClient::SessionStatus status = m_client.getStatus();
        if(!status.connected) {
            std::cout << "Connection lost!" << std::endl;
            break;
        }
        //headNode->rotate(Ogre::Vector3(0, 1, 0), Ogre::Radian(tps));
        //headNode->setPosition(reconLoc.getX(), 0, reconLoc.getZ());
        
        Ogre::WindowEventUtilities::messagePump();
        if(sf::Keyboard::isKeyPressed(KeyConfig::getInstance().moveForward)) {
            //reconLoc.requestSet(reconLoc.getX(), reconLoc.getZ() - tps * 5);
        }
        if(sf::Keyboard::isKeyPressed(KeyConfig::getInstance().moveBack)) {
            //reconLoc.requestSet(reconLoc.getX(), reconLoc.getZ() + tps * 5);
        }
        if(sf::Keyboard::isKeyPressed(KeyConfig::getInstance().moveLeft)) {
            //reconLoc.requestSet(reconLoc.getX() - tps * 5, reconLoc.getZ());
        }
        if(sf::Keyboard::isKeyPressed(KeyConfig::getInstance().moveRight)) {
            //reconLoc.requestSet(reconLoc.getX() + tps * 5, reconLoc.getZ());
        }
        if(m_window->isClosed()) {
            break;
        }
        
        if(!m_ogreRoot->renderOneFrame()) {
            break;
        }
    }
}



}