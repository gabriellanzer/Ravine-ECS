#ifndef COMPONENTSITERATOR_HPP
#define COMPONENTSITERATOR_HPP

#include "ComponentsGroup.hpp"

namespace rv
{
    template <typename TComp> struct CompGroupIt
    {
        // Iterator Group Fields
        TComp* offsets[50];
        int32_t sizes[50];
        // Current Group Iterator
        uint8_t count;

        constexpr CompGroupIt() : count(0) {}

        inline CompGroupIt(ComponentsGroup<TComp>** groups, const int32_t groupCount, TComp* data)
            : count(groupCount)
        {
            for (uint8_t i = 0; i < count; i++)
            {
                const ComponentsGroup<TComp>* group = groups[i];
                offsets[i] = data + group->baseOffset; // Start Data Ptr
                sizes[i] = group->size;    // Group Size
            }
        }
        ~CompGroupIt()
        {
            count = -1;
        }
    };
} // namespace rv

#endif