#ifndef BOUNDINGSYSTEM_HPP
#define BOUNDINGSYSTEM_HPP

#include "BaseSystem.hpp"
#include "PositionComponent.hpp"
#include "VelocityComponent.hpp"

class BoundingSystem : public BaseSystem<PositionComponent, VelocityComponent>
{
    virtual void update(double deltaTime, size_t size, PositionComponent *pos, VelocityComponent *vel)
    {
        for (size_t i = 0; i < size; i++)
        {
            if (pos[i].y < 0.0f)
            {
                pos[i].y = 0.0f;
                vel[i].Vy *= -1.0f;
            }
        }
    }
};

#endif