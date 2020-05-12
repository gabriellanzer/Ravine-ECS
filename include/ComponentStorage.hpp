#ifndef COMPONENTSTORAGE_HPP
#define COMPONENTSTORAGE_HPP

#include <map>
#include <set>
#include <stdlib.h>
#include <string.h>

#include "ComponentsGroup.hpp"
#include "ComponentsIterator.hpp"

namespace rv
{

    template <typename TComponent> class ComponentStorage
    {
        // Group def
        typedef ComponentsGroup<TComponent> ComponentsGroup;
        // Registry def
        typedef std::set<GroupMask, GroupMaskCmp> GroupMaskSet;
        typedef std::map<intptr_t, GroupMaskSet> GroupsRegistry;
        typedef typename GroupsRegistry::value_type GroupRegPair;
        typedef typename GroupsRegistry::iterator GroupsRegIt;
        // Groups Storage def
        typedef std::map<GroupMask, ComponentsGroup*, GroupMaskCmp> GroupsMap;
        typedef typename GroupsMap::value_type GroupMaskPair;
        typedef typename GroupsMap::iterator GroupIt;

      private:
        int32_t size = 0;
        int32_t capacity = 0;
        TComponent* data;

      public:
        /**
         * @brief Organized storage of groups based on their representative mask values.
         */
        GroupsMap groups;
        /**
         * @brief Organized storage of groups registry masks for each mask combination.
         */
        GroupsRegistry groupsRegistry;

        ComponentStorage() : capacity(10), data(static_cast<TComponent*>(malloc(10 * sizeof(TComponent)))) {}

        ~ComponentStorage()
        {
            free(data);
            groups.clear();
            capacity = 0;
        }

        void grow(int32_t newCapacity = 0)
        {
            const int32_t grow = max(capacity, newCapacity) * 1.5f;
            TComponent* newData = static_cast<TComponent*>(malloc(grow * sizeof(TComponent)));
            memcpy(newData, data, capacity * sizeof(TComponent));
            free(data);
            data = newData;
            capacity = grow;
        }

        ComponentsIterator<TComponent>* getComponentIterator(const intptr_t mask)
        {
            // Check if registry entry exists
            GroupsRegIt regIt = groupsRegistry.find(mask);
            if (regIt == groupsRegistry.end())
            {
                return nullptr;
            }

            // Create Iterator
            const int32_t groupCount = regIt->second.size();
            ComponentsGroup** groupsWithMask = new ComponentsGroup*[groupCount];
            int32_t i = 0;
            for (const GroupMask& mask : regIt->second)
            {
                // Perform Groups Lookup
                groupsWithMask[i] = groups[mask];
                ++i;
            }
            ComponentsIterator<TComponent>* it = new ComponentsIterator<TComponent>(groupsWithMask, groupCount, data);

            // Safe to perform cleanup now
            delete[] groupsWithMask;

            return it;
        }

        // TODO: Process many groups, each with different masks
        void addComponent(const intptr_t* masks, const int32_t maskCount, const TComponent* comps, int32_t count)
        {
            // Check if we have enough space
            if (size + count >= capacity)
            {
                grow();
            }

            GroupIt groupIt = getComponentGroup(masks, maskCount);

            // Make space for the new components
            GroupIt it = groupIt;
            for (it++; it != groups.end(); it++)
            {
                it->second->rollClockwise(count);
            }

            // Hold group reference
            ComponentsGroup* group = groupIt->second;

            // Make space for the new components in the group
            group->shiftClockwise(count);

            // Add the new components in the group
            group->addComponent(comps, count);

            // Increase Used Size
            size += count;
        }

        GroupIt getComponentGroup(const intptr_t* masks, const int32_t maskCount)
        {
            // Compute Group Mask
            GroupMask mask(masks, maskCount);

            // Get existing group
            GroupIt it = groups.lower_bound(mask);
            if (it != groups.end() && !(groups.key_comp()(mask, it->first)))
            {
                return it;
            }

            // Creates new Group
            it = groups.insert(it, GroupMaskPair(mask, nullptr));
            // Proper Initialization
            int32_t baseOffset = 0;
            if (it != groups.begin())
            {
                GroupIt lastGroupIt = it;
                --lastGroupIt;
                ComponentsGroup* lastGroup = lastGroupIt->second;
                baseOffset = lastGroup->baseOffset + lastGroup->size;
            }
            it->second = new ComponentsGroup(data, baseOffset);

            // Skip current ComponentType ptr
            const intptr_t curType = (intptr_t)getInstance();
            intptr_t* selComb = new intptr_t[maskCount - 1];
            for (int32_t i = 0, j = 0; i < maskCount - 1; i++, j++)
            {
                if (masks[j] == curType)
                {
                    j++;
                }
                selComb[i] = masks[j];
            }
            // Insert Group Mask in the registry
            GroupsRegIt regEntryIt = getRegistryEntryIt(curType);
            regEntryIt->second.insert(mask);
            // Insert Group Mask for all combinations
            int32_t combCount;
            intptr_t* combs = getMaskCombinations(selComb, maskCount - 1, combCount);
            for (int32_t i = 0; i < combCount; i++)
            {
                const intptr_t comb = curType + combs[i];
                regEntryIt = getRegistryEntryIt(comb);
                regEntryIt->second.insert(mask);
            }
            delete[] selComb;
            return it;
        }

        inline GroupsRegIt getRegistryEntryIt(const intptr_t mask)
        {
            // Look for registry entry
            GroupsRegIt regIt = groupsRegistry.lower_bound(mask);

            // If no registry entry exists
            if (regIt != groupsRegistry.end() && !(groupsRegistry.key_comp()(mask, regIt->first)))
            {
                return regIt;
            }

            // Create a new registry set on the corret position
            regIt = groupsRegistry.insert(regIt, GroupRegPair(mask, GroupMaskSet()));

            // Returns entry
            return regIt;
        }

        static ComponentStorage<TComponent> const* getInstance()
        {
            static ComponentStorage<TComponent>* storage = new ComponentStorage<TComponent>();
            return storage;
        }
    };

} // namespace rv

#endif