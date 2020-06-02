#ifndef GRAVITYSYSTEM_HPP
#define GRAVITYSYSTEM_HPP

#include "ravine/ecs.h"
#include "components/Position.h"
#include "components/Velocity.h"

using namespace rv;

class GravitySystem : public BaseSystem<Velocity>
{
  public:
    void update(double deltaTime, int32_t size, Velocity* const vel) override
    {
        for (int32_t i = 0; i < size; i++)
        {
            vel[i].y -= 9.8f * deltaTime * deltaTime;
        }
    }
};

#endif