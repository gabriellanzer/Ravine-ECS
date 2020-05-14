#ifndef MOVEMENTSYSTEM_H
#define MOVEMENTSYSTEM_H

#include "BaseSystem.hpp"
#include "PositionComponent.hpp"
#include "VelocityComponent.hpp"
#include <iostream>

using namespace rv;
using std::cout;
using std::endl;

class MovementSystem : public BaseSystem<VelocityComponent, PositionComponent>
{
    void update(double deltaTime, size_t size, ComponentsIterator<VelocityComponent> vel, ComponentsIterator<PositionComponent> pos) override;
};

#endif