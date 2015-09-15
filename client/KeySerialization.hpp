#ifndef SKM_KEYSERIALIZATION_HPP
#define SKM_KEYSERIALIZATION_HPP

#include <string>
#include "SFML/Window.hpp"

namespace skm {

namespace KeySerialization {
    sf::Keyboard::Key toKey(std::string str);
    std::string toString(sf::Keyboard::Key key);
};

}

#endif // SKM_KEYSERIALIZATION_HPP
