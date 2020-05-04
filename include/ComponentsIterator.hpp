#ifndef COMPONENTSITERATOR_HPP
#define COMPONENTSITERATOR_HPP

#include <set>

#include "ComponentsGroup.hpp"

using std::set;

namespace rv
{
    template <typename TComponent> struct ComponentsIterator
    {
        // Storage data pointer
        TComponent const* data;

        // Iterator Group Fields
        int32_t* sizes;
        int32_t* offsets;
        const int32_t groupCount;

        // Current Iterator Position (in fields)
        const int32_t it;

        ComponentsIterator(ComponentsGroup<TComponent>** groups, const int32_t groupCount,
                           TComponent const* storageData)
            : data(storageData), groupCount(groupCount), it(0)
        {
            sizes = static_cast<int32_t*>(malloc(groupCount));
            offsets = static_cast<int32_t*>(malloc(groupCount));
            for (int32_t i = 0; i < groupCount; i++)
            {
                ComponentsGroup<TComponent>* group = groups[i];
                sizes[i] = group->size;
                offsets[i] = group->baseOffset;
            }
        }
        ~ComponentsIterator()
        {
            free(sizes);
            free(offsets);
            groupCount = 0;
            data = nullptr;
            it = -1;
        }
        inline TComponent& operator[](int32_t id) { return data[id]; }
        inline const TComponent& operator[](int32_t id) const { return data[id]; }
    };
} // namespace rv

#endif