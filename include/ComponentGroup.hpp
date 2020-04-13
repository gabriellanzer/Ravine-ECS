#ifndef CYCLIC_ARRAY_HPP
#define CYCLIC_ARRAY_HPP
#include "FastMath.h"
#include <cstdlib>
#include <string>

namespace rv
{

template <typename TComponent> class ComponentGroup
{
    int32_t baseOffset = 0;
    int32_t size = 0;
    int32_t capacity = 0;
    int32_t tipOffset = 0;
    TComponent *data = nullptr;

  public:
    ComponentGroup() : capacity(10), data(static_cast<TComponent *>(malloc(10 * sizeof(TComponent)))) {}

    explicit ComponentGroup(const int32_t capacity)
        : capacity(capacity), data(static_cast<TComponent *>(malloc(capacity * sizeof(TComponent))))
    {
    }

    void grow(int32_t newCapacity = 0)
    {
        const int32_t grow = max(capacity, newCapacity) * 1.5f;
        TComponent *newData = static_cast<TComponent *>(malloc(grow * sizeof(TComponent)));
        memcpy(newData, data, capacity * sizeof(TComponent));
        free(data);
        data = newData;
        capacity = grow;
    }

    void addComponent(const TComponent *comps, const uint32_t count)
    {
        const int32_t missLeft = tipOffset - count;
        const int32_t rightMask = signMask(missLeft);
        const int32_t rightCount = rightMask * -missLeft;
        const int32_t leftCount = rightMask * tipOffset + (1 - rightMask) * count;

        if (size + count > capacity)
            grow();
        // Copy to the end of the list
        memcpy(data + baseOffset + size, comps + leftCount, sizeof(TComponent) * rightCount);
        // Copy components to the left of the tip
        memcpy(data + baseOffset + tipOffset - leftCount, comps + 0, sizeof(TComponent) * leftCount);

        size += rightCount;
    }

    void addComponent(const TComponent &comp) { addComponent(&comp, 1); }

    constexpr const TComponent *getDataIt() const { return data + baseOffset; }

    constexpr const TComponent *getDataIt(int32_t &size) const
    {
        size = this->size;
        return data + baseOffset;
    }

    constexpr int32_t getSize() const { return size; }

    // Should move base ptr, changes tipOffset, size is maintained
    constexpr void rollClockwise(const int32_t count)
    {
        const int32_t toCpy = min(size, count);
        const int32_t stride = max(size, count);
        if (baseOffset + stride + toCpy > capacity) // If there isn't enough space
        {
            grow(stride + toCpy);
        }
        memcpy(data + baseOffset + stride, data + baseOffset, toCpy); // Roll data
        tipOffset -= toCpy;                                           // Decrease tipOffset
        tipOffset += signMask(tipOffset) * size;                      // Wrap around

        // Should Increase base ptr
        baseOffset += toCpy;
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
        if (baseOffset + size + rollCount > capacity) // If there isn't enough space
        {
            grow(baseOffset + size + rollCount);
        }
        memcpy(data + baseOffset + size, data + baseOffset, rollCount);       // Roll data
        memcpy(data + baseOffset, data + baseOffset + rollCount, shiftCount); // Shift data
        size += count;                                                        // Increases size to update end of array
        return count;                                                         // Returns how many slots left before tip
    }

    /**
     * @brief Gets the debug string.
     *
     * @return const char*
     */
    const char *getDebugStr() const
    {
        const int32_t debugSize = capacity * 2;
        char *debugStr = new char[debugSize + 2];
        for (size_t debugIt = 0; debugIt < capacity; debugIt++)
        {
            debugStr[debugIt] = data[debugIt];
            if (debugIt < baseOffset || debugIt >= baseOffset + size)
            {
                debugStr[capacity + 1 + debugIt] = 'x';
            }
            else
            {
                debugStr[capacity + 1 + debugIt] = '~';
            }
        }
        debugStr[capacity] = '\n';
        debugStr[debugSize + 1] = '\0';
        debugStr[capacity + 1 + tipOffset] = '^';
        return debugStr;
    }
};

} // namespace rv

#endif