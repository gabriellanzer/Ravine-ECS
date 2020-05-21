#ifndef POSITIONCOMPONENT_H
#define POSITIONCOMPONENT_H

#include <iostream>
using std::ostream;

struct PositionComponent
{
    float x, y, z;

    constexpr PositionComponent() : x(0), y(0), z(0) {}

    constexpr PositionComponent(float x, float y, float z) : x(x), y(y), z(z) {}

    inline friend ostream& operator<<(ostream& os, const PositionComponent& pos)
    {
        os << "{ " << pos.x << "; " << pos.y << "; " << pos.z << "}";
        return os;
    }
};

#endif