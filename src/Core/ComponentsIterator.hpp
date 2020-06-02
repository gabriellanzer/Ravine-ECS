#ifndef COMPONENTSITERATOR_HPP
#define COMPONENTSITERATOR_HPP

#include "ComponentsGroup.hpp"

namespace rv
{
    template <typename TComp> struct CompIt
    {
      private:
        TComp* data;
        int32_t lSize;
        int32_t rSize;

      public:
        constexpr CompIt() : data(nullptr), lSize(0), rSize(0) {}
        constexpr CompIt(TComp* data, int32_t offset, int32_t size) : data(data), lSize(offset), rSize(size - offset) {}
        inline ~CompIt() {}

        constexpr int32_t getSize() const { return lSize + rSize; }

        TComp* const getChunk(int32_t id, int32_t& size)
        {
            if (id < rSize)
            {
                size = rSize;
                return &data[lSize + id];
            }
            else // (id >= rSize)
            {
                size = lSize;
                return &data[id - rSize];
            }
        }
    };

    template <typename TComp> struct CompGroupIt
    {
        // Iterator Group Fields
        CompIt<TComp> compIt[50];
        // Current Group Iterator
        uint8_t count;

        constexpr CompGroupIt() : count(0) {}

        inline CompGroupIt(ComponentsGroup<TComp>** groups, const int32_t groupCount, TComp* data) : count(groupCount)
        {
            for (uint8_t i = 0; i < count; i++)
            {
                const ComponentsGroup<TComp>* group = groups[i];
                compIt[i] = CompIt<TComp>(group->data + group->baseOffset, group->tipOffset, group->size);
            }
        }
        ~CompGroupIt() { count = -1; }
    };
} // namespace rv

#endif