#ifndef COMPONENTSTORAGE_HPP
#define COMPONENTSTORAGE_HPP

#include <map>
#include <set>
#include <stdlib.h>
#include <string.h>

#include "ComponentGroup.hpp"

namespace rv
{

template <typename TComponent> class ComponentStorage
{
    // Group def
    typedef ComponentGroup<TComponent> ComponentGroup;
    // Registry def
    typedef std::set<GroupMask, GroupMaskCmp> GroupMaskSet;
    typedef std::map<intptr_t, GroupMaskSet> GroupsRegistry;
    typedef typename GroupsRegistry::iterator GroupsRegIt;
    // Groups Storage def
    typedef std::map<GroupMask, ComponentGroup*, GroupMaskCmp> GroupsMap;
	typedef typename GroupsMap::value_type GroupMaskPair;
    typedef typename GroupsMap::iterator GroupIt;

  private:
    int32_t size = 0;
    int32_t capacity = 0;
    TComponent* data;

  public:
    /**
     * @brief Organized holding of groups based on their representative mask values.
     * Aka.:
     */
    GroupsMap groups;
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
        GroupIt it = groups.lower_bound(mask);
        if (it != groups.end() && !(groups.key_comp()(mask, it->first)))
        {
            return it;
        }

        // Create new Group
    	it = groups.insert(it, GroupMaskPair(mask, nullptr));

		// Proper Initialization
        int32_t baseOffset = 0;
		if(it != groups.begin())
		{
        	GroupIt lastGroupIt = it;
			--lastGroupIt;
			ComponentGroup* lastGroup = lastGroupIt->second;
			baseOffset = lastGroup->baseOffset + lastGroup->size;
		}
        it->second = new ComponentGroup(data, baseOffset);

        // TODO: Compute all combinations of masks and perform group registration
        // int32_t combCount;
        // intptr_t* combintations = getMaskCombinations(masks, maskCount, combCount);
        // for (int32_t i = 0; i < combCount; i++)
        // {
        // 	GroupsRegIt regIt = groupsRegistry.find();
        // }

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