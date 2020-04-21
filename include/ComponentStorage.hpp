#ifndef COMPONENTSTORAGE_HPP
#define COMPONENTSTORAGE_HPP

#include <cstdlib>
#include <hash_map>
#include <string>

#include "ComponentGroup.hpp"

using std::hash_map;

namespace rv
{

template <typename TComponent> class ComponentStorage
{
  private:
    int32_t capacity = 0;
    TComponent* data;
    int32_t groupsCount;
    intptr_t* groupMasks;
    intptr_t* groupsLength;
    ComponentGroup<TComponent>* groups;

  public:
    ComponentStorage() : capacity(10), data(static_cast<TComponent*>(malloc(10 * sizeof(TComponent)))) {}

    explicit ComponentStorage(const int32_t capacity)
        : capacity(capacity), data(static_cast<TComponent*>(malloc(capacity * sizeof(TComponent))))
    {
    }

    ~ComponentStorage()
    {
        free(data);
        free(groups);
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

    void addComponent(const TComponent* comps, int32_t count) { groups.addComponent(comps, count); }

    void addComponent(const intptr_t* masks, const int32_t maskCount, const TComponent* comps, int32_t count)
    {
        groups.addComponent(comps, count);
    }
};

template <typename TComponent> struct ComponentMaskList
{
    const intptr_t mask;
    const TComponent* comps;
    const int32_t count;
};

} // namespace rv

#endif