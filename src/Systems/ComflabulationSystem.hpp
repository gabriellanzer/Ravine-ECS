#ifndef COMFLABULATIONSYSTEM_HPP
#define COMFLABULATIONSYSTEM_HPP

#include "ravine/ecs.h"
#include "components/Comflabulation.h"

using namespace rv;

class ComflabulationSystem : public BaseSystem<Comflabulation>
{
    void update(double deltaTime, int32_t size, Comflabulation* const comflab) override
    {
        for (int32_t i = 0; i < size; i++)
        {
            Comflabulation& comf = comflab[i];
            comf.thingy *= 1.000001f;
            comf.mingy = !comf.mingy;
            comf.dingy++;
        }
    }
};

#endif