#ifndef COMPONENTSITERATOR_HPP
#define COMPONENTSITERATOR_HPP

#include <set>

#include "ComponentsGroup.hpp"

using std::set;

namespace rv
{
    template <typename TComponent> struct ComponentsIterator
    {
      private:
        // Storage data pointer
        TComponent* data;

        // Iterator Group Fields
        int32_t groupInfo[90 /*LIMIT OF 30 GROUPS*/];
        int32_t groupCount;

      public:
        // Current Group Iterator
        int32_t groupIt;
        // Components Count
        int32_t count;

        constexpr ComponentsIterator() : data(nullptr), groupCount(0), groupIt(0), count(0) {}
        ComponentsIterator(ComponentsGroup<TComponent>** groups, const int32_t groupCount, TComponent* storageData)
            : data(storageData), groupCount(groupCount), groupIt(0), count(0)
        {
            for (int32_t i = 0; i < groupCount; i++)
            {
                ComponentsGroup<TComponent>* group = groups[i];
                const int32_t offset = group->baseOffset;
                const int32_t size = group->size;
                groupInfo[i * 3 + 0] = count; // Initial Local Pos
                count += size;
                groupInfo[i * 3 + 1] = count - 1; // Final Local Pos
                groupInfo[i * 3 + 2] = offset;    // Initial Real Pos
            }
        }
        ~ComponentsIterator()
        {
            data = nullptr;
            groupIt = -1;
            count = -1;
        }

        inline TComponent& operator[](const int32_t id)
        {
            // Search for the right group
            for (int32_t i = 0; i < groupCount; i++)
            {
                const int32_t it = groupIt * 3;
                const int32_t iniPos = groupInfo[it + 0];
                const int32_t endPos = groupInfo[it + 1];
                const int lMask = -signMask(id - iniPos);
                const int rMask = signMask(endPos - id);
                groupIt += lMask + rMask;
            }
            // Calculate real position
            const int32_t it = groupIt * 3;
            const int32_t iniPos = groupInfo[it + 0];
            const int32_t offset = groupInfo[it + 2];
            const int32_t realPos = offset + (id - iniPos);
            return data[realPos];
        }

        inline const TComponent& operator[](const int32_t id) const
        {
            // Search for the right group
            for (int32_t i = 0; i < groupCount; i++)
            {
                const int32_t it = groupIt * 3;
                const int32_t iniPos = groupInfo[it + 0];
                const int32_t endPos = groupInfo[it + 1];
                const int lMask = -signMask(id - iniPos);
                const int rMask = signMask(endPos - id);
                groupIt += lMask + rMask;
            }
            // Calculate real position
            const int32_t it = groupIt * 3;
            const int32_t iniPos = groupInfo[it + 0];
            const int32_t offset = groupInfo[it + 2];
            const int32_t realPos = offset + (id - iniPos);
            return data[realPos];
        }
    };
} // namespace rv

#endif