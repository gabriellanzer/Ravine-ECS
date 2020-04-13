#ifndef GRAVITYSYSTEM_H
#define GRAVITYSYSTEM_H

#include "BaseSystem.hpp"
#include "VelocityComponent.hpp"

using namespace rv;

class GravitySystem : public BaseSystem<VelocityComponent>
{
    // Inherited via BaseSystem
    void update(double deltaTime, size_t size, VelocityComponent* vel) override;
};

#endif