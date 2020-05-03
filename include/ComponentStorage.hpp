#ifndef COMPONENTSTORAGE_HPP
#define COMPONENTSTORAGE_HPP

#include <cstdlib>
#include <map>
#include <string>

#include "ComponentGroup.hpp"

namespace rv
{

template <typename TComponent> class ComponentStorage
{
    typedef ComponentGroup<TComponent> ComponentGroup;
    typedef std::multimap<intptr_t, GroupMask> GroupsRegistry;
    typedef std::map<GroupMask, ComponentGroup*, GroupMaskCmp> GroupMaskMap;
    typedef typename GroupMaskMap::iterator GroupIt;

  private:
    int32_t size = 0;
    int32_t capacity = 0;
    TComponent* data;

  public:
    /**
     * @brief Organized holding of groups based on their representative mask values.
     * Aka.:
     */
    GroupMaskMap groups;
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

    GroupIt getComponentGroup(const intptr_t* masks, const int32_t maskCount)
    {
        // Compute Group Mask
        GroupMask mask(masks, maskCount);

        // Get existing group
        GroupIt it = groups.find(mask);
        if (it != groups.end())
        {
            return it;
        }

        // Create new Group
        ComponentGroup* group;
        it = groups.emplace(mask, group).first;

        // Setup
        const int32_t groupPos = std::distance(groups.begin(), it);
        int32_t baseOffset = 0;
        int32_t i = 0;
        for (auto maskGroupPair : groups)
        {
            if (i == groupPos - 1)
            {
                ComponentGroup* lastGroup = maskGroupPair.second;
                baseOffset = lastGroup->baseOffset + lastGroup->size;
                break;
            }
            i++;
        }
        it->second = new ComponentGroup(data, baseOffset);

        // TODO: Compute all combinations of masks and perform group registration

        return it;
    }

    void addComponent(const intptr_t* masks, const int32_t maskCount, const TComponent* comps, int32_t count)
    {
        // Check if we have enough space
        if (size + count >= capacity)
        {
            grow();
        }

        // TODO: Process many groups, each with different masks
        GroupIt groupIt = getComponentGroup(masks, maskCount);

        // Make space for the new components
        GroupIt it = groupIt;
        for (it++; it != groups.end(); it++)
        {
            it->second->rollClockwise(count);
        }

        // Hold group reference
        ComponentGroup* group = groupIt->second;

        // Make space for the new components in the group
        group->shiftClockwise(count);

        // Add the new components in the group
        group->addComponent(comps, count);

        // Increase Used Size
        size += count;
    }
};

} // namespace rv

#endif