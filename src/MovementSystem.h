#ifndef MOVEMENTSYSTEM_H
#define MOVEMENTSYSTEM_H
#include <iostream>

#include "BaseSystem.hpp"
#include "PositionComponent.h"
#include "VelocityComponent.h"
#include "taskflow/taskflow.hpp"

using namespace rv;
using std::cout;
using std::endl;

class MovementSystem : public BaseSystem<VelocityComponent, PositionComponent>
{
  public:
    void update(double deltaTime, size_t size, VelocityComponent* vel, PositionComponent* pos) override;
};

#endif