#ifndef MOVEMENTSYSTEM_H
#define MOVEMENTSYSTEM_H

#include "BaseSystem.hpp"
#include "PositionComponent.h"
#include "VelocityComponent.h"

using namespace rv;

class MovementSystem : public BaseSystem<VelocityComponent, PositionComponent>
{
  public:
    void update(double deltaTime, int32_t size, VelocityComponent* const vel, PositionComponent* const pos) override;
};

#endif