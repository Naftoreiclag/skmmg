#ifndef WORLD_HPP
#define WORLD_HPP

#include <vector>
#include "Entity.hpp"

namespace skm
{

class World
{
public:
    std::vector<Entity*> entities;

    World();
    ~World();

};

}

#endif // WORLD_HPP
