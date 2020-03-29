#pragma once
#include <iostream>
#include "BaseSystem.hpp"
#include "PositionComponent.hpp"
#include "VelocityComponent.hpp"

using std::cout;
using std::endl;

class MovementSystem : public BaseSystem<VelocityComponent, PositionComponent>
{
	void update(double deltaTime, size_t size, VelocityComponent* vel, PositionComponent* pos) override;
};