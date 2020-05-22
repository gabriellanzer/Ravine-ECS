#ifndef POSITIONCOMPONENT_H
#define POSITIONCOMPONENT_H

#include <iostream>
using std::ostream;

struct PositionComponent
{
    float x, y;

    constexpr PositionComponent() : x(0), y(0) {}

    constexpr PositionComponent(float x, float y, float z) : x(x), y(y) {}

    inline friend ostream& operator<<(ostream& os, const PositionComponent& pos)
    {
        os << "{ " << pos.x << "; " << pos.y << "}";
        return os;
    }
};

#endif