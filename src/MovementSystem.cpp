#include "MovementSystem.h"
#include <iostream>

void MovementSystem::update(double deltaTime, const size_t size, VelocityComponent* const vel, PositionComponent* const pos)
{
    for (size_t i = 0; i < size; i++)
    {
        const VelocityComponent& v = vel[i];
        PositionComponent& p = pos[i];
        p.x += v.x * deltaTime;
        p.y += v.y * deltaTime;
    }
}