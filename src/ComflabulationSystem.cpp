#include "ComflabulationSystem.h"
#include "taskflow/core/task.hpp"

void ComflabulationSystem::update(double deltaTime, size_t size, CompIt<ComflabulationComponent> comf)
{
    taskFlow->parallel_for(0, (int)size, 1, [comf](int i) {
        ComflabulationComponent& comflab = comf[i];
        comflab.thingy *= 1.000001f;
        comflab.mingy = !comflab.mingy;
        comflab.dingy++;
    }, 10'000);
}