#include "MovementSystem.h"
#include <iostream>

void MovementSystem::update(double deltaTime, const size_t size, CompIt<VelocityComponent> vel, CompIt<PositionComponent> pos)
{
    taskFlow->parallel_for(0, (int)size, 1, [deltaTime, vel, pos](int i) {
        const VelocityComponent& v = vel[i];
        PositionComponent& p = pos[i];
        p.x += v.x * deltaTime;
        p.y += v.y * deltaTime;
        p.z += v.z * deltaTime;
    }, 10'000);
}