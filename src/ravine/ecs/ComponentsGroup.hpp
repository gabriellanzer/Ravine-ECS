#ifndef COMPONENTS_GROUP_HPP
#define COMPONENTS_GROUP_HPP

#include <cstdlib>
#include <string>

#include "Entity.hpp"
#include "FastMath.h"

namespace rv
{
    template <class TComponent>
    struct ComponentsGroup
    {
        TComponent* const& data;
        int32_t baseOffset = 0;
        int32_t size = 0;
        int32_t tipOffset = 0;

        constexpr ComponentsGroup(TComponent* const& storageData, const int32_t storageOffset)
            : data(storageData), baseOffset(storageOffset)
        {
        }

        inline void addComponent(const TComponent* comps, const uint32_t count);

        inline void addComponent(const TComponent& comp);

        /**
         * @brief Removes the given components based on their Ids.
         *
         * @param compIds Sorted list (ascending) of Ids whose components will be removed.
         * @param count Size of the given component Ids list.
         * @return int32_t Amount of elements emptied to the right-side of the array.
         */
        inline int32_t remComponent(const int32_t* compIds, const int32_t count);

        inline void remComponent(const int32_t compId);

        inline TComponent* getComponent(const int32_t compId);

        /**
         * @brief Returns the last components (before tip).
         *
         * @return TComponent* The last component.
         */
        inline TComponent* getLastComponent();

        /**
         * @brief Rolls the components in a clockwise manner.
         * Moves base ptr, changes tip offset, size is maintained.
         *
         * @param count Amount of elements to roll clockwise.
         */
        inline void rollClockwise(const int32_t count);

        /**
         * @brief Rolls the components in a counter-clockwise manner.
         * Moves base ptr, changes tip offset, size is maintained.
         *
         * @param count Amount of elements to roll counter-clockwise.
         */
        inline void rollCounterClockwise(const int32_t count);

        /**
         * @brief Shifts the components in a clockwise manner.
         * Doesn't move base ptr, doesn't change tipOffset, size is increased.
         *
         * @param count Amount of elements to shift clockwise.
         * @return int32_t Amount of slots freed left of the tip.
         */
        inline int32_t shiftClockwise(int32_t count);

        /**
         * @brief Usefull shortcut for accessing group start ptr.
         *
         * @return TComponent* Group start data position ptr.
         */
        inline TComponent* dataPos();

        // TODO: Implement Shift CounterClockwise
        // TODO: Implement Swap of Components
        // TODO: Implement InsertComponent (on a specific location)
    };

    template <class TComponent>
    inline void ComponentsGroup<TComponent>::addComponent(const TComponent* comps, const uint32_t count)
    {
        const int32_t missLeft = tipOffset - count;
        const int32_t rightMask = signMask(missLeft);
        const int32_t rightCount = rightMask * -missLeft;
        const int32_t leftCount = rightMask * tipOffset + (1 - rightMask) * count;
        // Add components at group end
        memcpy(dataPos() + size, comps + 0, rightCount * sizeof(TComponent));
        // Add components before tip
        memcpy(dataPos() + tipOffset - leftCount, comps + rightCount, leftCount * sizeof(TComponent));
        size += rightCount;
    }

    template <>
    inline void ComponentsGroup<Entity>::addComponent(const Entity* comps, const uint32_t count)
    {
        const int32_t missLeft = tipOffset - count;
        const int32_t rightMask = signMask(missLeft);
        const int32_t rightCount = rightMask * -missLeft;
        const int32_t leftCount = rightMask * tipOffset + (1 - rightMask) * count;
        Entity* dst = dataPos() + size;
        memcpy(dst, comps + 0, rightCount * sizeof(Entity)); // Copy to the end of the group
        for (int32_t i = 0; i < rightCount; i++)             // Update Entity IDs
        {
            dst[i].id = size + i;
        }
        dst = dataPos() + tipOffset - leftCount;
        memcpy(dst, comps + rightCount, leftCount * sizeof(Entity)); // Copy components to the left of the tip
        for (int32_t i = 0; i < leftCount; i++)                      // Update Entity IDs
        {
            dst[i].id = size - leftCount + rightCount + i;
        }
        size += rightCount;
    }

    template <class TComponent>
    inline void ComponentsGroup<TComponent>::addComponent(const TComponent& comp)
    {
        addComponent(&comp, 1);
    }

    template <class TComponent>
    inline int32_t ComponentsGroup<TComponent>::remComponent(const int32_t* compIds, const int32_t count)
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
        for (int32_t i = leftComprCount - 1; i >= 0; i--)
        {
            const int32_t cId = i;
            const int32_t comprPos = compIds[cId];

            // Calculate compression shifts
            int32_t comprShifts = 1;
            for (int32_t j = cId - 1; j >= 0; j--)
            {
                const int32_t nextComprPos = compIds[cId - comprShifts];
                const int32_t fetchLimit = comprPos - comprShifts - 1;
                const int32_t incMask = signMask(fetchLimit - nextComprPos);
                comprShifts += incMask; // Increase shift count
                i -= incMask;           // Can skip next compression
            }

            // Actual position of the component without wrapping
            const int32_t actualPos = tipOffset + comprPos;

            // Calculate amount of elements to be compressed left
            const int32_t comprCount = size - actualPos - 1;

            // Perform compression by moving memory blocks
            const int32_t srcPos = actualPos + 1;
            const int32_t dstPos = srcPos - comprShifts;
            memmove(dataPos() + dstPos, dataPos() + srcPos, comprCount * sizeof(TComponent));
        }
        size -= leftComprCount;

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
            memmove(dataPos() + comprShifts, dataPos(), comprCount * sizeof(TComponent));
        }
        baseOffset += rightComprCount;
        tipOffset -= rightComprCount;
        size -= rightComprCount;

        // Roll counter-clockwise to fill removed spaces
        rollCounterClockwise(rightComprCount);

        return leftComprCount;
    }

    template <>
    inline int32_t ComponentsGroup<Entity>::remComponent(const int32_t* compIds, const int32_t count)
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
        for (int32_t i = leftComprCount - 1; i >= 0; i--)
        {
            const int32_t cId = i;
            const int32_t comprPos = compIds[cId];
            // Calculate compression shifts
            int32_t comprShifts = 1;
            for (int32_t j = cId - 1; j >= 0; j--)
            {
                const int32_t nextComprPos = compIds[cId - comprShifts];
                const int32_t fetchLimit = comprPos - comprShifts - 1;
                const int32_t incMask = signMask(fetchLimit - nextComprPos);
                comprShifts += incMask; // Increase shift count
                i -= incMask;           // Can skip next compression
            }
            // Actual position of the component without wrapping
            const int32_t actualPos = tipOffset + comprPos;
            // Calculate amount of elements to be compressed left
            const int32_t comprCount = size - actualPos - 1;
            // Perform compression by moving memory blocks
            const int32_t srcPos = actualPos + 1;
            const int32_t dstPos = srcPos - comprShifts;
            memmove(dataPos() + dstPos, dataPos() + srcPos, comprCount * sizeof(Entity));
            // Update entity ids
            for (int32_t i = 0; i < comprCount; i++)
            {
                data[baseOffset + dstPos + i].id -= comprShifts;
            }
        }
        size -= leftComprCount;

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
            memmove(dataPos() + comprShifts, dataPos(), comprCount * sizeof(Entity));
            // Update entity ids
            for (int32_t i = 0; i < comprCount; i++)
            {
                data[baseOffset + comprShifts + i].id -= (comprShifts + leftComprCount);
            }
        }
        baseOffset += rightComprCount;
        tipOffset -= rightComprCount;
        size -= rightComprCount;

        // Roll counter-clockwise to fill removed spaces
        rollCounterClockwise(rightComprCount);

        return leftComprCount;
    }

    template <class TComponent>
    inline void ComponentsGroup<TComponent>::remComponent(const int32_t compId)
    {
        remComponent(&compId, 1);
    }

    template <class TComponent>
    inline TComponent* ComponentsGroup<TComponent>::getComponent(const int32_t compId)
    {
        return dataPos() + (tipOffset + compId) % size;
    }

    template <class TComponent>
    inline TComponent* ComponentsGroup<TComponent>::getLastComponent()
    {
        return getComponent(size - 1);
    }

    template <class TComponent>
    inline void ComponentsGroup<TComponent>::rollClockwise(const int32_t count)
    {
        const int32_t toCopy = min(size, count);
        const int32_t stride = max(size, count);
        memcpy(dataPos() + stride, dataPos(), toCopy * sizeof(TComponent)); // Roll data
        tipOffset -= toCopy;                                                // Decrease tipOffset
        tipOffset += signMask(tipOffset) * size;                            // Wrap around

        // Should Increase base ptr
        baseOffset += count;
    }

    template <class TComponent>
    inline void ComponentsGroup<TComponent>::rollCounterClockwise(const int32_t count)
    {
        const int32_t dstOffset = min(baseOffset, count);
        const int32_t toCopy = min(dstOffset, size);
        const int32_t srcPos = size - toCopy;
        TComponent* dst = dataPos() - dstOffset;
        TComponent* src = dataPos() + srcPos;
        memcpy(dst, src, toCopy * sizeof(TComponent));      // Roll data
        tipOffset += toCopy;                                // Increase tipOffset
        tipOffset -= signMask(size - tipOffset - 1) * size; // Wrap around
        baseOffset -= toCopy;                               // Decrease base ptr
    }

    template <class TComponent>
    inline int32_t ComponentsGroup<TComponent>::shiftClockwise(int32_t count)
    {
        count = min(count, tipOffset);
        const int32_t mask = signMask(count - tipOffset - 1);
        const int32_t shiftCount = (tipOffset - count) * mask;
        const int32_t rollCount = count * mask;
        memcpy(dataPos() + size, dataPos(), rollCount * sizeof(TComponent));       // Roll data
        memcpy(dataPos(), dataPos() + rollCount, shiftCount * sizeof(TComponent)); // Shift data
        size += count; // Increases size to update end of array
        return count;  // Returns how many slots left before tip
    }

    template <class TComponent>
    TComponent* ComponentsGroup<TComponent>::dataPos()
    {
        return data + baseOffset;
    }

    /**
     * @brief Struct that represents the hash of component types.
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

        inline GroupMask(const intptr_t* masks, const int32_t count) : typePtr(0), typesCount(count)
        {
            for (size_t i = 0; i < typesCount; i++)
            {
                typePtr += masks[i];
            }
        }
    };

    /**
     * @brief Group Mask Compare operation.
     */
    struct GroupMaskCmp
    {
        inline bool operator()(const GroupMask& a, const GroupMask& b) const
        {
            if (a.typesCount != b.typesCount)
            {
                return a.typesCount > b.typesCount;
            }
            else
            {
                return a.typePtr < b.typePtr;
            }
        }
    };

} // namespace rv

#endif