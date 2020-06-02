#include "ComflabulationSystem.h"

void ComflabulationSystem::update(double dt, int32_t size, Comflabulation* const comflab)
{
    for (int32_t i = 0; i < size; i++)
    {
        Comflabulation& comf = comflab[i];
        comf.thingy *= 1.000001f;
        comf.mingy = !comf.mingy;
        comf.dingy++;
    }
}