#pragma once
#include "BaseSystem.hpp"
#include "VelocityComponent.hpp"

class GravitySystem : public BaseSystem<VelocityComponent>
{
	// Inherited via BaseSystem
	void update(double deltaTime, size_t size, VelocityComponent* vel) override;
};