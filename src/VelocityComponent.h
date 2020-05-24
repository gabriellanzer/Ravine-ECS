#ifndef VELOCITYCOMPONENT_H
#define VELOCITYCOMPONENT_H

#include <iostream>
using std::ostream;

struct VelocityComponent
{
    float x, y;

    constexpr VelocityComponent() : x(0), y(0) {}

    constexpr VelocityComponent(float x, float y, float z) : x(x), y(y) {}

    inline friend ostream& operator<<(ostream& os, const VelocityComponent& pos)
    {
        os << "{ " << pos.x << "; " << pos.y << "}";
        return os;
    }
};

#endif