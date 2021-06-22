#ifndef POSITION_H
#define POSITION_H

struct Position
{
    float x, y;

    constexpr Position() : x(0), y(0) {}

    constexpr Position(float x, float y) : x(x), y(y) {}
};

#endif