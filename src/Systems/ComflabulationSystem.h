#ifndef COMFLABULATIONSYSTEM_H
#define COMFLABULATIONSYSTEM_H

#include "Core/BaseSystem.hpp"
#include "Components/Comflabulation.h"

using namespace rv;

class ComflabulationSystem : public BaseSystem<Comflabulation>
{
    void update(double deltaTime, int32_t size, Comflabulation* const comflab) override;
};

#endif