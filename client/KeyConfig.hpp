#ifndef SKM_KEYCONFIG_HPP
#define SKM_KEYCONFIG_HPP

#include "SFML/Window.hpp"

namespace skm
{

class KeyConfig {
public:
    sf::Keyboard::Key moveForward;
    sf::Keyboard::Key moveLeft;
    sf::Keyboard::Key moveBack;
    sf::Keyboard::Key moveRight;

    KeyConfig();
    ~KeyConfig();
    
    static KeyConfig& getInstance();
};

}

#endif // SKM_KEYCONFIG_HPP
