#ifndef CYCLIC_ARRAY_HPP
#define CYCLIC_ARRAY_HPP
#include "FastMath.h"
#include <cstdlib>
#include <string>

namespace RV
{

template <typename TComponent> class CyclicArray
{
    int32_t size = 0;
    int32_t capacity = 0;
    int32_t tipOffset = 0;
    TComponent *data = nullptr;

  public:
    CyclicArray()
        : capacity(10),
          data(_ASSERT((this->data = static_cast<TComponent *>(calloc(10, sizeof(TComponent)))) != nullptr))
    {
    }

    explicit CyclicArray(const int32_t capacity)
        : capacity(capacity),
          data(_ASSERT((this->data = static_cast<TComponent *>(calloc(capacity, sizeof(TComponent)))) != nullptr))
    {
    }

    constexpr void reallocate(int32_t newCapacity = 0)
    {
        const int32_t grow = max(capacity, newCapacity) * 1.5f;
        TComponent *oldData = data;
        _ASSERT((data = static_cast<TComponent *>(calloc(grow, sizeof(TComponent)))) != nullptr);
        _ASSERT(memcpy(data, oldData, sizeof(TComponent) * capacity) != nullptr);
        capacity = grow;
        free(oldData);
    }

    int32_t addComponent(const TComponent *comps, const uint32_t count)
    {
        if (size + count > capacity)
            reallocate();

        const int32_t addPosition = size;
        _ASSERT(memcpy(data + addPosition, comps, sizeof(TComponent) * count) != nullptr);
        size += count;

        return addPosition;
    }

    int32_t addComponent(const TComponent &comp) { return addComponent(&comp, 1); }

    constexpr const TComponent *getData() const { return this->data; }

    constexpr const TComponent *getData(int32_t &size) const
    {
        size = this->size;
        return this->data;
    }

    constexpr int32_t getSize() const { return this->size; }

    // Should move base ptr, changes tipOffset, size is maintained
    constexpr void rollClockwise(const int32_t count)
    {
        const int32_t toCpy = min(size, count);
        const int32_t stride = max(size, count);
        if (stride + toCpy > capacity) // If there isn't enough space
        {
            reallocate(stride + toCpy);
        }
        memcpy(data + stride, data, toCpy);      // Roll data
        tipOffset -= toCpy;                      // Decrease tipOffset
        tipOffset += signMask(tipOffset) * size; // Wrap around

        // Should Increase base ptr
        // data += toCpy;
    }

    /**
     * @brief Shifts the components in a clockwise manner.
     * Doesn't move base ptr, doesn't change tipOffset, size is increased.
     *
     * @param count
     * @return constexpr int32_t
     */
    constexpr int32_t shiftClockwise(int32_t count)
    {
        count = min(count, tipOffset);
        const int32_t mask = signMask(count - tipOffset);
        const int32_t shiftCount = (tipOffset - count) * mask;
        const int32_t rollCount = count * mask;
        if (size + rollCount > capacity) // If there isn't enough space
        {
            reallocate(size + rollCount);
        }
        memcpy(data + size, data, rollCount);       // Roll data
        memcpy(data, data + rollCount, shiftCount); // Shift data
        size += count;                              // Increases size to update end of array
        return count;                               // Returns how many slots left before tip
    }

    const char *getDebugStr() const
    {
        const debugSize = size * 2;
        const char *debugStr = new char[debugSize + 1];
        debugStr[debugSize] = '\0';
        for (size_t it = 0; it < debugSize; it++)
        {
        }
        return debugStr;
    }
};

} // namespace RV

#endif