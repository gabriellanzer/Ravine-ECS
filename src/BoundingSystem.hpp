#ifndef BOUNDINGSYSTEM_HPP
#define BOUNDINGSYSTEM_HPP

#include "BaseSystem.hpp"
#include "PositionComponent.hpp"
#include "VelocityComponent.hpp"

using namespace rv;

class BoundingSystem : public BaseSystem<PositionComponent, VelocityComponent>
{
    virtual void update(double deltaTime, size_t size, ComponentsIterator<PositionComponent> pos, ComponentsIterator<VelocityComponent> vel)
    {
        for (size_t i = 0; i < size; i++)
        {
            if (pos[i].y < 0.0f)
            {
                pos[i].y = 0.0f;
                vel[i].y *= -1.0f;
            }
        }
    }
};

#endif