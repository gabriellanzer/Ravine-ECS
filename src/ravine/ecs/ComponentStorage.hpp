#ifndef COMPONENTSTORAGE_HPP
#define COMPONENTSTORAGE_HPP

#include <map>
#include <set>
#include <stdlib.h>
#include <string.h>

#include "ComponentsGroup.hpp"
#include "ComponentsIterator.hpp"
#include "IComponentStorage.h"

namespace rv
{
	// Empty Namespace to avoid leaking using directives
	namespace
	{
		// Registry def
		using GroupMaskSet = std::set<GroupMask, GroupMaskCmp>;
		using GroupsRegistry = std::map<intptr_t, GroupMaskSet>;
		using GroupRegPair = GroupsRegistry::value_type;
		using GroupsRegIt = GroupsRegistry::iterator;
		// Groups Storage def
		template <typename TComp>
		using CompGroup = ComponentsGroup<TComp>;
		template <typename TComp>
		using GroupsMap = std::map<GroupMask, CompGroup<TComp>*, GroupMaskCmp>;
		template <typename TComp>
		using GroupMaskPair = typename GroupsMap<TComp>::value_type;
		template <typename TComp>
		using GroupIt = typename GroupsMap<TComp>::iterator;
		template <typename TComp>
		using GroupCIt = typename GroupsMap<TComp>::const_iterator;

		template <typename TComp>
		class ComponentStorage : public IComponentStorage
		{

		  private:
			int32_t size = 0;
			int32_t capacity = 0;

		  public:
			TComp* data;

			/**
			 * @brief Organized storage of groups based on their representative mask values.
			 */
			GroupsMap<TComp> groups;
			/**
			 * @brief Organized storage of groups registry masks for each mask combination.
			 */
			GroupsRegistry groupsRegistry;

			ComponentStorage() : capacity(10), data((TComp*)malloc(10 * sizeof(TComp))) {}

			~ComponentStorage()
			{
				free(data);
				groups.clear();
				capacity = 0;
			}

			inline void grow(int32_t newCapacity = 0);

			inline CompGroupIt<TComp> getComponentIterator(const intptr_t mask);

			// TODO: Process many groups, each with different masks
			inline CompGroup<TComp>* addComponent(const intptr_t* masks, const int32_t maskCount,
							      const TComp* comps, int32_t count);

			inline TComp* addComponent(const intptr_t* masks, const int32_t maskCount, const TComp& comp);

			inline GroupIt<TComp> getComponentGroup(const intptr_t* masks, const int32_t maskCount);

			inline GroupsRegIt getRegistryEntryIt(const intptr_t mask);

			inline static ComponentStorage<TComp>* getInstance();

			void swapComponent(int32_t entityId, GroupMask oldTypeMask, GroupMask newTypeMask) final
			{
				// TODO: Implement Swapping
			}

			void removeComponent(int32_t entityId, GroupMask typeMask) final;

			void removeComponents(GroupIdList groupIdList) final;
		};

		template <class TComp>
		inline void ComponentStorage<TComp>::grow(int32_t newCapacity)
		{
			const int32_t grow = max(capacity, newCapacity) * 1.2f;
			TComp* newData = (TComp*)malloc(grow * sizeof(TComp));
			memcpy(newData, data, capacity * sizeof(TComp));
			free(data);
			data = newData;
			capacity = grow;
		}

		template <class TComp>
		inline CompGroupIt<TComp> ComponentStorage<TComp>::getComponentIterator(const intptr_t mask)
		{
			// Check if registry entry exists
			GroupsRegIt regIt = groupsRegistry.find(mask);
			if (regIt == groupsRegistry.end())
			{
				return CompGroupIt<TComp>();
			}

			// Create Iterator
			const int32_t groupCount = regIt->second.size();
			CompGroup<TComp>** groupsWithMask = new CompGroup<TComp>*[groupCount];
			int32_t i = 0;
			for (const GroupMask& mask : regIt->second)
			{
				// Perform Groups Lookup
				groupsWithMask[i] = groups[mask];
				++i;
			}
			CompGroupIt<TComp> it(groupsWithMask, groupCount, data);

			// Safe to perform cleanup now
			delete[] groupsWithMask;

			return it;
		}

		// TODO: Process many groups, each with different masks
		template <class TComp>
		inline ComponentsGroup<TComp>* ComponentStorage<TComp>::addComponent(const intptr_t* masks,
										     const int32_t maskCount,
										     const TComp* comps, int32_t count)
		{
			// Check if we have enough space
			if (size + count >= capacity)
			{
				grow();
			}

			GroupIt<TComp> groupIt = getComponentGroup(masks, maskCount);

			// Make space for the new components
			GroupIt<TComp> it = groups.end();
			for (it--; it != groupIt; it--)
			{
				it->second->rollClockwise(count);
			}

			// Hold group reference
			CompGroup<TComp>* group = groupIt->second;

			// Make space for the new components in the group
			group->shiftClockwise(count);

			// Add the new components in the group
			group->addComponent(comps, count);

			// Increase Used Size
			size += count;

			return group;
		}

		template <class TComp>
		inline TComp* ComponentStorage<TComp>::addComponent(const intptr_t* masks, const int32_t maskCount,
								    const TComp& comp)
		{
			ComponentsGroup<TComp>* group = addComponent(masks, maskCount, &comp, 1);

			// Return Component Reference
			return group->getLastComponent();
		}

		template <class TComp>
		inline GroupIt<TComp> ComponentStorage<TComp>::getComponentGroup(const intptr_t* masks,
										 const int32_t maskCount)
		{
			// Compute Group Mask
			GroupMask mask(masks, maskCount);

			// Get existing group
			GroupIt<TComp> it = groups.lower_bound(mask);
			if (it != groups.end() && !(groups.key_comp()(mask, it->first)))
			{
				return it;
			}

			// Creates new Group
			it = groups.insert(it, GroupMaskPair<TComp>(mask, nullptr));
			// Proper Initialization
			int32_t baseOffset = 0;
			if (it != groups.begin())
			{
				GroupIt<TComp> lastGroupIt = it;
				--lastGroupIt;
				CompGroup<TComp>* lastGroup = lastGroupIt->second;
				baseOffset = lastGroup->baseOffset + lastGroup->size;
			}
			it->second = new CompGroup<TComp>(data, baseOffset);

			// Skip current ComponentType ptr
			const intptr_t curType = (intptr_t)getInstance();
			intptr_t* selComb = new intptr_t[maskCount - 1];
			for (int32_t i = 0, j = 0; i < maskCount - 1; i++, j++)
			{
				if (masks[j] == curType)
				{
					j++;
				}
				selComb[i] = masks[j];
			}
			// Insert Group Mask in the registry
			GroupsRegIt regEntryIt = getRegistryEntryIt(curType);
			regEntryIt->second.insert(mask);
			// Insert Group Mask for all combinations
			int32_t combCount;
			intptr_t* combs = getMaskCombinations(selComb, maskCount - 1, combCount);
			for (int32_t i = 0; i < combCount; i++)
			{
				const intptr_t comb = curType + combs[i];
				regEntryIt = getRegistryEntryIt(comb);
				regEntryIt->second.insert(mask);
			}
			delete[] combs;
			delete[] selComb;
			return it;
		}

		template <class TComp>
		inline GroupsRegIt ComponentStorage<TComp>::getRegistryEntryIt(const intptr_t mask)
		{
			// Look for registry entry
			GroupsRegIt regIt = groupsRegistry.lower_bound(mask);

			// Check if a registry entry exists
			if (regIt != groupsRegistry.end() && !(groupsRegistry.key_comp()(mask, regIt->first)))
			{
				return regIt;
			}

			// Create a new registry set on the corret position
			regIt = groupsRegistry.insert(regIt, GroupRegPair(mask, GroupMaskSet()));

			// Returns entry
			return regIt;
		}

		template <class TComp>
		inline ComponentStorage<TComp>* ComponentStorage<TComp>::getInstance()
		{
			static ComponentStorage<TComp>* storage = new ComponentStorage<TComp>();
			return storage;
		}

		template <class TComp>
		inline void ComponentStorage<TComp>::removeComponent(int32_t entityId, GroupMask typeMask)
		{
			GroupIt<TComp> it = groups.find(typeMask);
			_ASSERT(it != groups.end());
			// Remove Component from specific group
			(*it->second).remComponent(&entityId, 1);
			// Roll all effected groups to fill the gap
			for (it++; it != groups.end(); it++)
			{
				(*it->second).rollCounterClockwise(1);
			}
		}

		template <class TComp>
		inline void ComponentStorage<TComp>::removeComponents(GroupIdList groupIdList)
		{
			// For the chuck removal we keep track of the next removal group (nextIt),
			// This allows us to go through effected groups only once, while accumulating
			// the roll ammount, so all groups fill-in the gaps left by removed components.
			int32_t accRoll = 0;
			GroupIdList::iterator beg = groupIdList.begin();
			GroupIt<TComp> nextIt = groups.find(beg->first);
			while (beg != groupIdList.end())
			{
				GroupIt<TComp> it = nextIt;
				_ASSERT(it != groups.end());

				// Remove Component from specific group
				std::vector<int>* entityIds = beg->second;
				int32_t roll = (*it->second).remComponent(entityIds->data(), entityIds->size());
				
				// Fill-in the gaps for the group whose components have been removed
				(*it->second).rollCounterClockwise(accRoll);
				accRoll += roll; // Accumulate the gaps for multiple groups removal
				
				// Get next group mask
				beg++;
				// And get next group it
				nextIt = (beg == groupIdList.end()) ? groups.end() : groups.find(beg->first);

				// Roll all effected groups to fill the gap (until next group removal)
				for (it++; it != nextIt; it++)
				{
					(*it->second).rollCounterClockwise(accRoll);
				}
			}
		}

	} // namespace
} // namespace rv

#endif