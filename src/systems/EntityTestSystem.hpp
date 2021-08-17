#ifndef ENTITYTESTSYSTEM_HPP
#define ENTITYTESTSYSTEM_HPP

#include "components/Position.h"
#include "ravine/ecs.h"
#include "ravine/ecs/Entity.hpp"

using namespace rv;

class EntityTestSystem : public BaseSystem<EntityProxy, Position>
{
	void update(double deltaTime, int32_t size, EntityProxy* const e, Position* const p) final
	{
		for (int32_t i = 0; i < size; i++)
		{
			fprintf(stdout, "|Ent(%ui)|Pos(%.3f,%.3f)", e[i].uniqueId, p[i].x, p[i].y);
		}
	}
};

#endif