#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <stdint.h>
#include <stdio.h>

using std::array;

namespace rv
{

    struct Entity
    {
        int32_t id;
        intptr_t* compTypes;

        constexpr Entity() : id(0), compTypes(nullptr) { }

        inline Entity(int32_t entity_id, intptr_t* masks, int32_t maskCount) : id(entity_id)
        {
            compTypes = new intptr_t[]
        }

        void print() { fprintf(stdout, "Entity(%i)", id); }
    };

} // namespace rv

#endif