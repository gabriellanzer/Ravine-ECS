#ifndef GRAVITYSYSTEM_HPP
#define GRAVITYSYSTEM_HPP

#include "Components/Position.h"
#include "Components/Velocity.h"
#include "Ravine/Ecs.h"

using namespace rv;

class GravitySystem : public BaseSystem<Velocity>
{
  public:
	void update(double deltaTime, int32_t size, Velocity* const vel) final
	{
		for (int32_t i = 0; i < size; i++)
		{
			vel[i].y -= 9.8f * deltaTime * deltaTime;
		}
	}
};

#endif