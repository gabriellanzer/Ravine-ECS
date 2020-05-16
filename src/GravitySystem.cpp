#include "GravitySystem.h"

void GravitySystem::update(double deltaTime, size_t size, ComponentsIterator<VelocityComponent> vel)
{
    for (size_t i = 0; i < size; i++)
    {
        vel[i].y -= 9.8 * deltaTime;
    }
}
