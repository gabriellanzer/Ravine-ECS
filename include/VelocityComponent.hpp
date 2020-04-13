#ifndef VELOCITYCOMPONENT_HPP
#define VELOCITYCOMPONENT_HPP

#include <iostream>
using std::ostream;

struct VelocityComponent
{
    float Vx, Vy, Vz;

    VelocityComponent() { Vx = Vy = Vz = 1; }

    VelocityComponent(float Vx, float Vy, float Vz) : Vx(Vx), Vy(Vy), Vz(Vz) {}

    VelocityComponent& operator*=(const double& rhs)
    {

        Vx *= rhs;
        Vy *= rhs;
        Vz *= rhs;
        return *this;
    }

    VelocityComponent operator*(const double& rhs)
    {
        VelocityComponent comp;
        comp.Vx = Vx * rhs;
        comp.Vy = Vy * rhs;
        comp.Vz = Vz * rhs;
        return comp;
    }

    VelocityComponent& operator+=(const VelocityComponent& rhs)
    {

        Vx += rhs.Vx;
        Vy += rhs.Vy;
        Vz += rhs.Vz;
        return *this;
    }

    friend ostream& operator<<(ostream& os, const VelocityComponent& vel)
    {
        os << "{ " << vel.Vx << "; " << vel.Vy << "; " << vel.Vz << "}";
        return os;
    }
};

#endif