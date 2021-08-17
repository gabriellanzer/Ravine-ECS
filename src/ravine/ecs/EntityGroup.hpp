#ifndef ENTITY_GROUP_HPP
#define ENTITY_GROUP_HPP

#include "ComponentsGroup.hpp"
#include "Entity.hpp"
#include "FastMath.h"

#include <cstdlib>
#include <stdint.h>
#include <string>
#include <vector>

namespace rv
{
	struct EntityLookup
	{
		uint32_t entityId;
		int32_t groupPos;

		inline bool operator<(const EntityLookup& other) { return entityId < other.entityId; }
	};

	template <>
	struct ComponentsGroup<EntityProxy>
	{
		using LookupList = std::vector<EntityLookup>;

		EntityProxy* const& data;
		int32_t baseOffset = 0;
		int32_t size = 0;
		int32_t tipOffset = 0;
		LookupList lookupBuffer;

		/**
		 * @brief Constructs a group from a storage data array pointer reference
		 *  and the group base offset position with respect to that array start.
		 *
		 */
		ComponentsGroup(EntityProxy* const& storageData, const int32_t storageOffset)
		    : data(storageData), baseOffset(storageOffset)
		{
		}

		/**
		 * @brief Batch adding of component to this group.
		 *
		 * @param comps Component data array to copy from.
		 * @param count Amount of components in the array.
		 */
		inline void addComponent(const EntityProxy* comps, const uint32_t count);

		/**
		 * @brief Adds a single component to this group.
		 *
		 * @param comp Component data to copy from.
		 */
		inline void addComponent(const EntityProxy& comp);

		/**
		 * @brief Removes the given components based on their Ids.
		 *
		 * @param compIds Sorted list (ascending) of Ids whose components will be removed.
		 * @param count Size of the given component Ids list.
		 * @return int32_t Amount of elements emptied to the right-side of the array.
		 */
		inline int32_t remComponent(const int32_t* compIds, const int32_t count);

		/**
		 * @brief Get a pointer to the component that matches the given id.
		 *
		 * @param compId Id of the requested component.
		 * @return EntityProxy* Pointer for the component address.
		 */
		inline EntityProxy* getComponent(const int32_t compId);

		/**
		 * @brief Returns the last components (before tip).
		 *
		 * @return EntityProxy* The last component.
		 */
		inline EntityProxy* getLastComponent();

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
		 * @return EntityProxy* Group start data position ptr.
		 */
		inline EntityProxy* dataPos();

		/**
		 * @brief Cleanup the buffer for a new pass.
		 *
		 */
		inline void flushLookupBuffer();
	};

	inline void ComponentsGroup<EntityProxy>::addComponent(const EntityProxy* comps, const uint32_t count)
	{
		const int32_t missLeft = tipOffset - count;
		// If there is any missing slots left of the tip
		const int32_t rightMask = signMask(missLeft);
		// Right count is how much it is missing at left of the tip
		const int32_t rightCount = rightMask * -missLeft;
		// Left count is either the whole space until the tip or the count of comps
		// (when there is no missing slots left of the tip)
		const int32_t leftCount = rightMask * tipOffset + (1 - rightMask) * count;

		// Pre-allocate lookup buffer
		lookupBuffer.reserve(lookupBuffer.size() + rightCount + leftCount);

		// Copy new components to their correct spots
		EntityProxy* dst = dataPos() + size;
		memcpy(dst, comps + 0, rightCount * sizeof(EntityProxy)); // Copy to the end of the group
		for (int32_t i = 0; i < rightCount; i++)		  // Update EntityProxy IDs
		{
			EntityProxy& entity = dst[i];
			entity.groupId = size + i;

			// TODO: Check if we can keep track only of entities the user wants to
			// Pay for what you use
			// if (entity.IsTracked())
			{
				lookupBuffer.push_back({entity.uniqueId, entity.groupId});
			}
		}
		dst = dataPos() + tipOffset - leftCount;
		memcpy(dst, comps + rightCount,
		       leftCount * sizeof(EntityProxy)); // Copy components to the left of the tip
		for (int32_t i = 0; i < leftCount; i++)	 // Update EntityProxy IDs
		{
			EntityProxy& entity = dst[i];
			entity.groupId = size - leftCount + rightCount + i;

			// TODO: Check if we can keep track only of entities the user wants to
			// Pay for what you use
			// if (entity.IsTracked())
			{
				lookupBuffer.push_back({entity.uniqueId, entity.groupId});
			}
		}
		size += rightCount;
	}

	inline void ComponentsGroup<EntityProxy>::addComponent(const EntityProxy& comp) { addComponent(&comp, 1); }

	inline int32_t ComponentsGroup<EntityProxy>::remComponent(const int32_t* compPos, const int32_t count)
	{
		const int32_t rightSize = size - tipOffset;
		int32_t leftComprCount = 0;

		// Count the number of left compressions
		for (int32_t i = 0; i < count; i++)
		{
			leftComprCount += signMask(compPos[i] - rightSize);
		}

		// Count the number of right compressions
		int32_t rightComprCount = count - leftComprCount;

		// Compress left all elements right of the tip
		for (int32_t i = leftComprCount - 1; i >= 0; i--)
		{
			const int32_t cId = i;
			const int32_t comprPos = compPos[cId];
			// Calculate compression shifts
			int32_t comprShifts = 1;
			for (int32_t j = cId - 1; j >= 0; j--)
			{
				const int32_t nextComprPos = compPos[cId - comprShifts];
				const int32_t fetchLimit = comprPos - comprShifts - 1;
				if (signMask(fetchLimit - nextComprPos) == 0)
				{
					break;
				}
				comprShifts++; // Increase shift count
				i--;	       // Can skip next compression
			}
			// Actual position of the component without wrapping
			const int32_t actualPos = tipOffset + comprPos;
			// Calculate amount of elements to be compressed left
			const int32_t comprCount = size - actualPos - 1;
			// Perform compression by moving memory blocks
			const int32_t srcPos = actualPos + 1;
			const int32_t dstPos = srcPos - comprShifts;
			memmove(dataPos() + dstPos, dataPos() + srcPos, comprCount * sizeof(EntityProxy));

			// Update entity ids
			lookupBuffer.reserve(lookupBuffer.size() + comprCount);
			for (int32_t i = 0; i < comprCount; i++)
			{
				EntityProxy& entity = data[baseOffset + dstPos + i];
				entity.groupId -= comprShifts;

				// TODO: Check if we can keep track only of entities the user wants to
				// Pay for what you use
				// if (entity.IsTracked())
				{
					lookupBuffer.push_back({entity.uniqueId, entity.groupId});
				}
			}
		}
		size -= leftComprCount;

		// Compress right all elements left of the tip
		for (int32_t i = leftComprCount; i < count; i++)
		{
			const int32_t cId = i;
			const int32_t comprPos = compPos[cId];
			// Calculate compression shifts
			int32_t comprShifts = 1;
			for (int32_t j = cId + 1; j < count; j++)
			{
				const int32_t nextComprPos = compPos[cId + comprShifts];
				const int32_t fetchLimit = comprPos + comprShifts + 1;
				if (signMask(nextComprPos - fetchLimit) == 0)
				{
					break;
				}
				comprShifts++; // Increase shift count
				i++;	       // Can skip next compression
			}
			// Calculate amount of elements to be compressed right
			const int32_t comprCount = comprPos - rightSize;
			// Perform compression by moving memory blocks
			memmove(dataPos() + comprShifts, dataPos(), comprCount * sizeof(EntityProxy));

			// Update entity ids
			lookupBuffer.reserve(lookupBuffer.size() + comprCount);
			for (int32_t i = 0; i < comprCount; i++)
			{
				EntityProxy& entity = data[baseOffset + comprShifts + i];
				entity.groupId -= (comprShifts + leftComprCount);

				// TODO: Check if we can keep track only of entities the user wants to
				// Pay for what you use
				// if (entity.IsTracked())
				{
					lookupBuffer.push_back({entity.uniqueId, entity.groupId});
				}
			}
		}
		baseOffset += rightComprCount;
		tipOffset -= rightComprCount;
		size -= rightComprCount;

		// Roll counter-clockwise to fill removed spaces
		rollCounterClockwise(rightComprCount);

		return leftComprCount;
	}

	inline EntityProxy* ComponentsGroup<EntityProxy>::getComponent(const int32_t compId)
	{
		return dataPos() + (tipOffset + compId) % size;
	}

	inline EntityProxy* ComponentsGroup<EntityProxy>::getLastComponent() { return getComponent(size - 1); }

	inline void ComponentsGroup<EntityProxy>::rollClockwise(const int32_t count)
	{
		const int32_t toCopy = min(size, count);
		const int32_t stride = max(size, count);
		memcpy(dataPos() + stride, dataPos(), toCopy * sizeof(EntityProxy)); // Roll data
		tipOffset -= toCopy;						     // Decrease tipOffset
		tipOffset += signMask(tipOffset) * size;			     // Wrap around

		// Should Increase base ptr
		baseOffset += count;
	}

	inline void ComponentsGroup<EntityProxy>::rollCounterClockwise(const int32_t count)
	{
		const int32_t dstOffset = min(baseOffset, count);
		const int32_t toCopy = min(dstOffset, size);
		const int32_t srcPos = size - toCopy;
		EntityProxy* dst = dataPos() - dstOffset;
		EntityProxy* src = dataPos() + srcPos;
		memcpy(dst, src, toCopy * sizeof(EntityProxy));	    // Roll data
		tipOffset += toCopy;				    // Increase tipOffset
		tipOffset -= signMask(size - tipOffset - 1) * size; // Wrap around
		baseOffset -= toCopy;				    // Decrease base ptr
	}

	inline int32_t ComponentsGroup<EntityProxy>::shiftClockwise(int32_t count)
	{
		count = min(count, tipOffset);
		const int32_t mask = signMask(count - tipOffset - 1);
		const int32_t shiftCount = (tipOffset - count) * mask;
		const int32_t rollCount = count * mask;
		memcpy(dataPos() + size, dataPos(), rollCount * sizeof(EntityProxy));	    // Roll data
		memcpy(dataPos(), dataPos() + rollCount, shiftCount * sizeof(EntityProxy)); // Shift data
		size += count; // Increases size to update end of array
		return count;  // Returns how many slots left before tip
	}

	inline EntityProxy* ComponentsGroup<EntityProxy>::dataPos() { return data + baseOffset; }

	inline void ComponentsGroup<EntityProxy>::flushLookupBuffer() { (void)lookupBuffer.empty(); }

} // namespace rv

#endif