#ifndef ENTITY_HPP
#define ENTITY_HPP

#include "Vec3f.hpp"

namespace skm
{

class Entity {
public:
    Vec3f location;

    Entity();
    ~Entity();
};

}

#endif // ENTITY_HPP
