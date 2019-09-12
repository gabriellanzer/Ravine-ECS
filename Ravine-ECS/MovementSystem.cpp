#include "MovementSystem.h"
#include <iostream>

void MovementSystem::update(double deltaTime, size_t size, VelocityComponent* vel, PositionComponent* pos)
{
	for (size_t i = 0; i < size; i++)
	{
		pos[i] += vel[i] * deltaTime;
		std::cout << "pos " << pos[i] << "; vel " << vel[i] << std::endl;
	}
	std::cout << size << std::endl;
}