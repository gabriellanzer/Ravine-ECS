#ifndef VELOCITY_H
#define VELOCITY_H

struct Velocity
{
    float x, y;

    constexpr Velocity() : x(0), y(0) {}

    constexpr Velocity(float x, float y) : x(x), y(y) {}
};

#endif