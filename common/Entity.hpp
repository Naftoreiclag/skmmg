#ifndef ENTITY_HPP
#define ENTITY_HPP

#include "Vec3f.hpp"
#include "SFML/System.hpp"

namespace skm
{

class Entity {
public:
    typedef sf::Uint32 Handle;
public:
    Vec3f m_location;
    
    const Handle m_handle;

    Entity(const Handle& handle);
    virtual ~Entity();
};

}

#endif // ENTITY_HPP
