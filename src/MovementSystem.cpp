#include "MovementSystem.h"
#include <iostream>

void MovementSystem::update(double deltaTime, int32_t size, VelocityComponent* const vel, PositionComponent* const pos)
{
    for (int32_t i = 0; i < size; i++)
    {
        const VelocityComponent& v = vel[i];
        PositionComponent& p = pos[i];
        p.x += v.x * deltaTime;
        p.y += v.y * deltaTime;
    }
}