#ifndef MOVEMENTSYSTEM_H
#define MOVEMENTSYSTEM_H

#include "BaseSystem.hpp"
#include "PositionComponent.hpp"
#include "VelocityComponent.hpp"
#include <iostream>

using std::cout;
using std::endl;

class MovementSystem : public BaseSystem<VelocityComponent, PositionComponent>
{
    void update(double deltaTime, size_t size, VelocityComponent *vel, PositionComponent *pos) override;
};

#endif