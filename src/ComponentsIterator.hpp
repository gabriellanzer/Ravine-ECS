#ifndef COMPONENTSITERATOR_HPP
#define COMPONENTSITERATOR_HPP

#include <set>

#include "ComponentsGroup.hpp"

using std::set;

namespace rv
{
    template <typename TComp> struct CompIt
    {
      private:
        // Storage data pointer
        TComp* data;

        // Iterator Group Fields
        int32_t groupInfo[90 /*LIMIT OF 30 GROUPS*/];
        int32_t groupCount;

      public:
        // Current Group Iterator
        int32_t groupIt;
        // Components Count
        int32_t count;

        constexpr CompIt() : data(nullptr), groupCount(0), groupIt(0), count(0) {}

        inline CompIt(ComponentsGroup<TComp>** groups, const int32_t groupCount, TComp* storageData)
            : data(storageData), groupCount(groupCount), groupIt(0), count(0)
        {
            for (int32_t i = 0; i < groupCount; i++)
            {
                ComponentsGroup<TComp>* group = groups[i];
                const int32_t offset = group->baseOffset;
                const int32_t size = group->size;
                groupInfo[i * 3 + 0] = count; // Initial Local Pos
                count += size;
                groupInfo[i * 3 + 1] = count - 1; // Final Local Pos
                groupInfo[i * 3 + 2] = offset;    // Initial Real Pos
            }
        }
        ~CompIt()
        {
            data = nullptr;
            groupIt = -1;
            count = -1;
        }

        constexpr TComp& operator[](const int32_t id)
        {
            // Search for the right group
            int32_t gCount = groupCount;
            for (int32_t i = 0; i < gCount; i++)
            {
                int lMask = signMask(id - groupInfo[groupIt + 0]);
                int rMask = signMask(groupInfo[groupIt + 1] - id);
                groupIt += (-lMask + rMask) * 3;
                gCount *= lMask | rMask;
            }
            return data[groupInfo[groupIt + 2] + (id - groupInfo[groupIt + 0])];
        }

        constexpr TComp& operator[](const int32_t id) const
        {
            // Search for the right group
            int32_t groupIt = 0;
            int32_t gCount = groupCount;
            for (int32_t i = 0; i < gCount; i++)
            {
                int lMask = signMask(id - groupInfo[groupIt + 0]);
                int rMask = signMask(groupInfo[groupIt + 1] - id);
                groupIt += (-lMask + rMask) * 3;
                gCount *= lMask | rMask;
            }
            return data[groupInfo[groupIt + 2] + (id - groupInfo[groupIt + 0])];
        }
    };
} // namespace rv

#endif