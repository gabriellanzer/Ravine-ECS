#ifndef ENTITYTESTSYSTEM_HPP
#define ENTITYTESTSYSTEM_HPP

#include "Components/Position.h"
#include "Ravine/Ecs.h"

using namespace rv;

class EntityTestSystem : public BaseSystem<EntityProxy, Position>
{
	void update(double deltaTime, int32_t size, EntityProxy* const e, Position* const p) final
	{
		for (int32_t i = 0; i < size; i++)
		{
			fprintf(stdout, "|Ent(%u)|Pos(%.3f,%.3f)", e[i].entityId, p[i].x, p[i].y);
		}
	}
};

#endif