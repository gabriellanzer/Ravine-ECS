#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <stdint.h>
#include <stdio.h>
#include <array>

using std::array;

namespace rv
{

    struct Entity
    {
        int32_t id;
        int32_t typesCount;
        intptr_t* compTypes;

        constexpr Entity() : id(0), typesCount(0), compTypes(nullptr) { }

        Entity(const Entity& other) : id(other.id), typesCount(other.typesCount)
        {
            compTypes = new intptr_t[typesCount];
            memcpy(compTypes, other.compTypes, sizeof(intptr_t) * typesCount);
        }

        Entity(Entity&& other) : id(other.id), typesCount(other.typesCount), compTypes(other.compTypes)
        {
            other.id = -1;
            other.typesCount = 0;
            other.compTypes = nullptr;
        }

        ~Entity()
        {
            id = -1;
            typesCount = 0;
            delete[] compTypes;
        }

        void print() { fprintf(stdout, "Entity(%i)", id); }
    };

} // namespace rv

#endif