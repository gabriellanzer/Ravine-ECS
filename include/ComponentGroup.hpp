#ifndef CYCLIC_ARRAY_HPP
#define CYCLIC_ARRAY_HPP

#include <cstdlib>
#include <string>

#include "FastMath.h"

namespace rv
{

template <typename TComponent> class ComponentGroup
{
  private:
    int32_t baseOffset = 0;
    int32_t size = 0;
    int32_t capacity = 0;
    int32_t tipOffset = 0;
    TComponent* data = nullptr;

    inline TComponent* dataPos()
    {
        return data + baseOffset;
    }

  public:
    ComponentGroup() : capacity(10), data(static_cast<TComponent*>(malloc(10 * sizeof(TComponent)))) {}

    explicit ComponentGroup(const int32_t capacity)
        : capacity(capacity), data(static_cast<TComponent*>(malloc(capacity * sizeof(TComponent))))
    {
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

    void addComponent(const TComponent* comps, const uint32_t count)
    {
        const int32_t missLeft = tipOffset - count;
        const int32_t rightMask = signMask(missLeft);
        const int32_t rightCount = rightMask * -missLeft;
        const int32_t leftCount = rightMask * tipOffset + (1 - rightMask) * count;

        // Check for capacity
        if (size + count > capacity)
        {
            grow();
        }

        // Copy to the end of the group
        memcpy(dataPos() + size, comps + leftCount, sizeof(TComponent) * rightCount);
        // Copy components to the left of the tip
        memcpy(dataPos() + tipOffset - leftCount, comps + 0, sizeof(TComponent) * leftCount);

        size += rightCount;
    }

    inline void addComponent(const TComponent& comp) { addComponent(&comp, 1); }

    /**
     * @brief Removes the given components based on their Ids.
     *
     * @param compIds Sorted list (ascending) of Ids whose components will be removed.
     * @param count Size of the given component Ids list.
     * @return int32_t Amount of elements emptied to the right-side of the array.
     */
    int32_t remComponent(const int32_t* compIds, const int32_t count)
    {
        const int32_t rightSize = size - tipOffset;
        int32_t leftComprCount = 0;

        // Count the number of left compressions
        for (int32_t i = 0; i < count; i++)
        {
            leftComprCount += signMask(compIds[i] - rightSize);
        }

        // Count the number of right compressions
        int32_t rightComprCount = count - leftComprCount;

        // Compress left all elements right of the tip 

        // Compress right all elements left of the tip
        for (int32_t i = leftComprCount; i < count; i++)
        {
            const int32_t cId = i;
            const int32_t comprPos = compIds[cId];

            // Calculate compression shifts
            int32_t comprShifts = 1;
            for (int32_t j = cId + 1; j < count; j++)
            {
                const int32_t nextComprPos = compIds[cId + comprShifts];
                const int32_t fetchLimit = comprPos + comprShifts + 1;
                const int32_t incMask = signMask(nextComprPos - fetchLimit);
                comprShifts += incMask; // Increase shift count
                i += incMask;           // Can skip next compression
            }

            // Calculate amount of elements to be compressed right
            const int32_t comprCount = comprPos - rightSize;

            // Perform compression by moving memory blocks
            memmove(dataPos() + comprShifts, dataPos(), comprCount);
        }
        baseOffset += rightComprCount;
        tipOffset  -= rightComprCount;
        size -= rightComprCount;

        // Roll counter-clockwise to fill removed spaces
        rollCounterClockwise(rightComprCount);

        return leftComprCount;
    }

    inline void remComponent(const int compId) { remComponent(&compId, 1); }

    // Should move base ptr, changes tipOffset, size is maintained
    void rollClockwise(const int32_t count)
    {
        const int32_t toCopy = min(size, count);
        const int32_t stride = max(size, count);
        if (baseOffset + stride + toCopy > capacity) // If there isn't enough space
        {
            grow(stride + toCopy);
        }
        memcpy(dataPos() + stride, dataPos(), toCopy);  // Roll data
        tipOffset -= toCopy;                            // Decrease tipOffset
        tipOffset += signMask(tipOffset) * size;        // Wrap around

        // Should Increase base ptr
        baseOffset += toCopy;
    }

    // Should move base ptr, changes tipOffset, size is maintained
    void rollCounterClockwise(const int32_t count)
    {
        const int32_t dstOffset = min(baseOffset, count);
        const int32_t toCopy = min(dstOffset, size);
        const int32_t srcPos = size - toCopy;
        TComponent* dst = dataPos() - dstOffset;
        TComponent* src = dataPos() + srcPos;
        memcpy(dst, src, toCopy);                       // Roll data
        tipOffset += toCopy;                            // Increase tipOffset
        tipOffset -= signMask(size - tipOffset - 1) * size; // Wrap around

        // Should Decrease base ptr
        baseOffset -= toCopy;
    }

    /**
     * @brief Shifts the components in a clockwise manner.
     * Doesn't move base ptr, doesn't change tipOffset, size is increased.
     *
     * @param count
     * @return constexpr int32_t
     */
    int32_t shiftClockwise(int32_t count)
    {
        count = min(count, tipOffset);
        const int32_t mask = signMask(count - tipOffset);
        const int32_t shiftCount = (tipOffset - count) * mask;
        const int32_t rollCount = count * mask;
        if (baseOffset + size + rollCount > capacity) // If there isn't enough space
        {
            grow(baseOffset + size + rollCount);
        }
        memcpy(dataPos() + size, dataPos(), rollCount);       // Roll data
        memcpy(dataPos(), dataPos() + rollCount, shiftCount); // Shift data
        size += count;                                                        // Increases size to update end of array
        return count;                                                         // Returns how many slots left before tip
    }

    inline const TComponent* getDataIt() const { return dataPos(); }

    inline const TComponent* getDataIt(int32_t& size) const
    {
        size = this->size;
        return dataPos();
    }

    inline int32_t getSize() const { return size; }

    /**
     * @brief Gets the debug string.
     *
     * @return const char*
     */
    const char* getDebugStr() const
    {
        const int32_t debugSize = capacity * 2;
        char* debugStr = new char[debugSize + 2];
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
        debugStr[capacity + 1 + baseOffset + tipOffset] = '^';
        return debugStr;
    }
};

/**
 * @brief Struct that represents the group hash
 */
struct GroupMask
{
    /**
     * @brief Hash of all pointer types this mask represents.
     */
    intptr_t typePtr;
    /**
     * @brief Amount of types this mask represents.
     */
    int32_t typesCount;
};

} // namespace rv

#endif