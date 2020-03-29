#ifndef POSITIONCOMPONENT_HPP
#define POSITIONCOMPONENT_HPP

#include "VelocityComponent.hpp"
#include <iostream>
using std::ostream;

struct PositionComponent
{
    float x, y, z;

    PositionComponent() { x = y = z = 1; }

    PositionComponent(float x, float y, float z) : x(x), y(y), z(z) {}

    PositionComponent &operator*=(const double &rhs)
    {

        x *= rhs;
        y *= rhs;
        z *= rhs;
        return *this;
    }

    PositionComponent operator*(const double &rhs)
    {
        PositionComponent comp;
        comp.x = x * rhs;
        comp.y = y * rhs;
        comp.z = z * rhs;
        return comp;
    }

    PositionComponent &operator+=(const PositionComponent &rhs)
    {

        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }

    PositionComponent &operator+=(const VelocityComponent &rhs)
    {

        x += rhs.Vx;
        y += rhs.Vy;
        z += rhs.Vz;
        return *this;
    }

    friend ostream &operator<<(ostream &os, const PositionComponent &pos)
    {
        os << "{ " << pos.x << "; " << pos.y << "; " << pos.z << "}";
        return os;
    }
};

#endif