#ifndef COMFLABULATIONSYSTEM_H
#define COMFLABULATIONSYSTEM_H

#include "BaseSystem.hpp"
#include "ComflabulationComponent.h"

using namespace rv;

class ComflabulationSystem : public BaseSystem<ComflabulationComponent>
{
    void update(double deltaTime, int32_t size, ComflabulationComponent* const comflab) override;
};

#endif