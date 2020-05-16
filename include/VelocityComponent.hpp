#ifndef VELOCITYCOMPONENT_HPP
#define VELOCITYCOMPONENT_HPP

#include <iostream>
using std::ostream;

struct VelocityComponent
{
    float x, y, z;

    VelocityComponent(): x(0), y(0), z(0) { }

    VelocityComponent(float Vx, float Vy, float Vz) : x(Vx), y(Vy), z(Vz) {}

    constexpr VelocityComponent& operator*=(const double& rhs)
    {

        x *= rhs;
        y *= rhs;
        z *= rhs;
        return *this;
    }

    inline VelocityComponent operator*(const double& rhs)
    {
        VelocityComponent comp;
        comp.x = x * rhs;
        comp.y = y * rhs;
        comp.z = z * rhs;
        return comp;
    }

    constexpr VelocityComponent& operator+=(const VelocityComponent& rhs)
    {

        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }

    friend ostream& operator<<(ostream& os, const VelocityComponent& vel)
    {
        os << "{ " << vel.x << "; " << vel.y << "; " << vel.z << "}";
        return os;
    }
};

#endif