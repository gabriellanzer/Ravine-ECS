#include "MovementSystem.h"
#include <iostream>

void MovementSystem::update(double deltaTime, int32_t size, Velocity* const vel, Position* const pos)
{
    for (int32_t i = 0; i < size; i++)
    {
        pos[i].x += vel[i].x * deltaTime;
        pos[i].y += vel[i].y * deltaTime;
    }
}