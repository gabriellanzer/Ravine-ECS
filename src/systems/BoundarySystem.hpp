#ifndef BOUNDARYSYSTEM_HPP
#define BOUNDARYSYSTEM_HPP

#include "components/Position.h"
#include "components/Velocity.h"
#include "ravine/ecs.h"

using namespace rv;

class BoundarySystem : public BaseSystem<Velocity, Position>
{
  public:
	void update(double deltaTime, int32_t size, Velocity* const vel, Position* const pos) final
	{
		for (int32_t i = 0; i < size; i++)
		{
			if (pos[i].y > 0)
			{
				continue;
			}
			pos[i].y = 0;
			vel[i].y *= -1;
		}
	}
};

#endif