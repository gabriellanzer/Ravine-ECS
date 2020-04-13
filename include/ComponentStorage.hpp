#ifndef COMPONENTSTORAGE_HPP
#define COMPONENTSTORAGE_HPP

#include <cstdlib>
#include <string>

#include "ComponentGroup.hpp"

namespace rv
{

template <typename TComponent> class ComponentStorage
{
  private:
    int32_t capacity = 0;
    TComponent* data;
    ComponentGroup<TComponent>* groups;

  public:
    ComponentStorage() : capacity(10), data(static_cast<TComponent*>(malloc(10 * sizeof(TComponent)))) {}

    explicit ComponentStorage(const int32_t capacity)
        : capacity(capacity), data(static_cast<TComponent*>(malloc(capacity * sizeof(TComponent))))
    {
    }

    ~ComponentStorage() { free(data); }

    void grow(int32_t newCapacity = 0)
    {
        const int32_t grow = max(capacity, newCapacity) * 1.5f;
        TComponent* newData = static_cast<TComponent*>(malloc(grow * sizeof(TComponent)));
        memcpy(newData, data, capacity * sizeof(TComponent));
        free(data);
        data = newData;
        capacity = grow;
    }
};

} // namespace rv

#endif