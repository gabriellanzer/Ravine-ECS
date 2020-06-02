#ifndef MOVEMENTSYSTEM_H
#define MOVEMENTSYSTEM_H

#include "Core/BaseSystem.hpp"
#include "Components/Position.h"
#include "Components/Velocity.h"

using namespace rv;

class MovementSystem : public BaseSystem<Velocity, Position>
{
  public:
    void update(double deltaTime, int32_t size, Velocity* const vel, Position* const pos) override;
};

#endif