#include "MovementSystem.h"
#include <iostream>

void MovementSystem::update(const double deltaTime, const size_t size, ComponentsIterator<VelocityComponent> vel, ComponentsIterator<PositionComponent> pos)
{
    for (size_t i = 0; i < size; i++)
    {
        pos[i] += vel[i] * deltaTime;
        // std::cout << "pos " << pos[i] << "; vel " << vel[i] << std::endl;
    }
    // system("cls");
}