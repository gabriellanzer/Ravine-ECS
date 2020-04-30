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
    typedef std::map<GroupMask, ComponentGroup, GroupMaskCmp> GroupMaskMap;

  private:
    int32_t capacity = 0;
    TComponent* data;

  public:
    GroupMaskMap groups;
    ComponentStorage() : capacity(10), data(static_cast<TComponent*>(malloc(10 * sizeof(TComponent)))) {}

    explicit ComponentStorage(const int32_t capacity)
        : capacity(capacity), data(static_cast<TComponent*>(malloc(capacity * sizeof(TComponent))))
    {
    }

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

    ComponentGroup& getComponentGroup(const intptr_t* masks, const int32_t maskCount)
    {
        // Compute Group Mask
        intptr_t resMask = masks[0];
        for (int32_t i = 1; i < maskCount; i++)
        {
            resMask ^= masks[i];
        }

        GroupMask mask = GroupMask{resMask, maskCount};
        
        // Get existing group
        typename GroupMaskMap::iterator it = groups.find(mask);
        if (it != groups.end())
        {
            return it->second;
        }

        // Create new Group
        ComponentGroup group;
        it = groups.emplace(mask, group).first;
        return it->second;
    }

    // void addComponent(const TComponent* comps, int32_t count) { groups.addComponent(comps, count); }

    // void addComponent(const intptr_t* masks, const int32_t maskCount, const TComponent* comps, int32_t count)
    // {
    //     intptr_t combinedMask groups.addComponent(comps, count);
    // }
};

} // namespace rv

#endif