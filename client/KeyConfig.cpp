#include "KeyConfig.hpp"

#include <fstream>
#include <sstream>

#include "rapidxml.hpp"

#include "KeySerialization.hpp"

namespace skm
{

KeyConfig::KeyConfig() {
    rapidxml::xml_document<> doc;
    
    {
        std::ifstream file("keys.xml");
        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();
        std::string data(buffer.str());
        doc.parse<0>(&data[0]);
    }
    
    rapidxml::xml_node<>* rootNode = doc.first_node("keys");
    
    rapidxml::xml_node<>* n_moveForward = rootNode->first_node("moveForward");
    rapidxml::xml_node<>* n_moveLeft = rootNode->first_node("moveLeft");
    rapidxml::xml_node<>* n_moveBack = rootNode->first_node("moveBack");
    rapidxml::xml_node<>* n_moveRight = rootNode->first_node("moveRight");
    
    if(n_moveForward) { moveForward = KeySerialization::toKey(n_moveForward->value()); }
    if(n_moveLeft   ) { moveLeft    = KeySerialization::toKey(n_moveLeft   ->value()); }
    if(n_moveBack   ) { moveBack    = KeySerialization::toKey(n_moveBack   ->value()); }
    if(n_moveRight  ) { moveRight   = KeySerialization::toKey(n_moveRight  ->value()); }
}

KeyConfig::~KeyConfig() {
}

KeyConfig& KeyConfig::getInstance() {
    static KeyConfig instance;
    return instance;
}


}

