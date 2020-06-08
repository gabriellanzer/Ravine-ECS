#ifndef ENTITYTESTSYSTEM_HPP
#define ENTITYTESTSYSTEM_HPP

#include "ravine/ecs.h"
#include "components/Position.h"

using namespace rv;

class EntityTestSystem : public BaseSystem<Entity, Position>
{
    void update(double deltaTime, int32_t size, Entity* const e, Position* const p) final
    {
        for (int32_t i = 0; i < size; i++)
        {
            fprintf(stdout, "|Ent(%i)|Pos(%.3f,%.3f)", e[i].id, p[i].x, p[i].y);
        }
    }
};

#endif