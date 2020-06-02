#ifndef ENTITYTESTSYSTEM_HPP
#define ENTITYTESTSYSTEM_HPP

#include "Core/BaseSystem.hpp"
#include "Core/Entity.hpp"
#include "Components/Position.h"

using namespace rv;

class EntityTestSystem : public BaseSystem<Entity, Position>
{
    void update(double deltaTime, int32_t size, Entity* const entity, Position* const pos) override
    {
        for (int32_t i = 0; i < size; i++)
        {
            const Entity& e = entity[i];
            const Position& p = pos[i];
            fprintf(stdout, "|Ent(%i)|Pos(%.3f,%.3f)", e.id, p.x, p.y);
        }
    }
};

#endif