#ifndef _GRAVITYSYSTEM_H
#define _GRAVITYSYSTEM_H

#include "BaseSystem.hpp"
#include "VelocityComponent.h"

using namespace rv;

class GravitySystem : public BaseSystem<VelocityComponent>
{
    // Inherited via BaseSystem
    void update(double deltaTime, size_t size, CompIt<VelocityComponent> vel) override;
};

#endif