#ifndef __ENTITYSTORAGE__H__
#define __ENTITYSTORAGE__H__

#include "ComponentStorage.hpp"
#include "Entity.hpp"
#include "EntityGroup.hpp"

#include <algorithm>
#include <vector>

namespace rv
{
	using LookupList = std::vector<EntityLookup>;

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

		template <>
		class ComponentStorage<EntityProxy> : public IComponentStorage
		{

		  private:
			int32_t size = 0;
			int32_t capacity = 0;

		  public:
			EntityProxy* data;

			/**
			 * @brief Organized storage of groups based on their representative mask values.
			 */
			GroupsMap<EntityProxy> groups;
			/**
			 * @brief Organized storage of groups registry masks for each mask combination.
			 */
			GroupsRegistry groupsRegistry;

			ComponentStorage() : capacity(10), data((EntityProxy*)malloc(10 * sizeof(EntityProxy))) {}

			~ComponentStorage()
			{
				free(data);
				groups.clear();
				capacity = 0;
			}

			inline void grow(int32_t newCapacity = 0);

			inline CompGroupIt<EntityProxy> getComponentIterator(const intptr_t mask);

			// TODO: Process many groups, each with different masks
			inline CompGroup<EntityProxy>* addComponent(const intptr_t* masks, const int32_t maskCount,
								    const EntityProxy* comps, int32_t count);

			inline EntityProxy* addComponent(const intptr_t* masks, const int32_t maskCount,
							 const EntityProxy& comp);

			inline GroupIt<EntityProxy> getComponentGroup(const intptr_t* masks, const int32_t maskCount);

			inline GroupsRegIt getRegistryEntryIt(const intptr_t mask);

			inline void flushEntityLookups(void (*callback)(const LookupList&));

			inline static ComponentStorage<EntityProxy>* getInstance();

			inline void swapComponent(int32_t entityId, GroupMask oldTypeMask, GroupMask newTypeMask) final
			{
				// TODO: Implement Swapping
			}

			inline void removeComponent(int32_t entityId, GroupMask typeMask) final;

			inline void removeComponents(GroupIdList groupIdList) final;
		};

		inline void ComponentStorage<EntityProxy>::grow(int32_t newCapacity)
		{
			const int32_t grow = max(capacity, newCapacity) * 1.2f;
			EntityProxy* newData = (EntityProxy*)malloc(grow * sizeof(EntityProxy));
			memcpy(newData, data, capacity * sizeof(EntityProxy));
			free(data);
			data = newData;
			capacity = grow;
		}

		inline CompGroupIt<EntityProxy> ComponentStorage<EntityProxy>::getComponentIterator(const intptr_t mask)
		{
			// Check if registry entry exists
			GroupsRegIt regIt = groupsRegistry.find(mask);
			if (regIt == groupsRegistry.end())
			{
				return CompGroupIt<EntityProxy>();
			}

			// Create Iterator
			const int32_t groupCount = regIt->second.size();
			CompGroup<EntityProxy>** groupsWithMask = new CompGroup<EntityProxy>*[groupCount];
			int32_t i = 0;
			for (const GroupMask& mask : regIt->second)
			{
				// Perform Groups Lookup
				groupsWithMask[i] = groups[mask];
				++i;
			}
			CompGroupIt<EntityProxy> it(groupsWithMask, groupCount, data);

			// Safe to perform cleanup now
			delete[] groupsWithMask;

			return it;
		}

		// TODO: Process many groups, each with different masks
		inline ComponentsGroup<EntityProxy>*
		ComponentStorage<EntityProxy>::addComponent(const intptr_t* masks, const int32_t maskCount,
							    const EntityProxy* comps, int32_t count)
		{
			// Check if we have enough space
			if (size + count >= capacity)
			{
				grow();
			}

			GroupIt<EntityProxy> groupIt = getComponentGroup(masks, maskCount);

			// Make space for the new components
			GroupIt<EntityProxy> it = groups.end();
			for (it--; it != groupIt; it--)
			{
				it->second->rollClockwise(count);
			}

			// Hold group reference
			CompGroup<EntityProxy>* group = groupIt->second;

			// Make space for the new components in the group
			group->shiftClockwise(count);

			// Add the new components in the group
			group->addComponent(comps, count);

			// Increase Used Size
			size += count;

			return group;
		}

		inline EntityProxy* ComponentStorage<EntityProxy>::addComponent(const intptr_t* masks,
										const int32_t maskCount,
										const EntityProxy& comp)
		{
			ComponentsGroup<EntityProxy>* group = addComponent(masks, maskCount, &comp, 1);

			// Return Component Reference
			return group->getLastComponent();
		}

		inline GroupIt<EntityProxy> ComponentStorage<EntityProxy>::getComponentGroup(const intptr_t* masks,
											     const int32_t maskCount)
		{
			// Compute Group Mask
			GroupMask mask(masks, maskCount);

			// Get existing group
			GroupIt<EntityProxy> it = groups.lower_bound(mask);
			if (it != groups.end() && !(groups.key_comp()(mask, it->first)))
			{
				return it;
			}

			// Creates new Group
			it = groups.insert(it, GroupMaskPair<EntityProxy>(mask, nullptr));
			// Proper Initialization
			int32_t baseOffset = 0;
			if (it != groups.begin())
			{
				GroupIt<EntityProxy> lastGroupIt = it;
				--lastGroupIt;
				CompGroup<EntityProxy>* lastGroup = lastGroupIt->second;
				baseOffset = lastGroup->baseOffset + lastGroup->size;
			}
			it->second = new CompGroup<EntityProxy>(data, baseOffset);

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

		inline GroupsRegIt ComponentStorage<EntityProxy>::getRegistryEntryIt(const intptr_t mask)
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

		inline void ComponentStorage<EntityProxy>::flushEntityLookups(void (*callback)(const LookupList&))
		{
			// TODO: Profile accumulation of entity lookups on a single structure
			// TODO: Handle cases where there are more than a single lookup per entity
			for (GroupCIt<EntityProxy> it = groups.cbegin(); it != groups.cend(); it++)
			{
				std::vector<EntityLookup>& lookupBuf = it->second->lookupBuffer;
				std::sort(lookupBuf.begin(), lookupBuf.end());
				callback(lookupBuf);
				(void)lookupBuf.empty();
			}
		}

		inline ComponentStorage<EntityProxy>* ComponentStorage<EntityProxy>::getInstance()
		{
			static ComponentStorage<EntityProxy>* storage = new ComponentStorage<EntityProxy>();
			return storage;
		}

		inline void ComponentStorage<EntityProxy>::removeComponent(int32_t entityId, GroupMask typeMask)
		{
			GroupIt<EntityProxy> it = groups.find(typeMask);
			_ASSERT(it != groups.end());
			// Remove Component from specific group
			(*it->second).remComponent(&entityId, 1);
			// Roll all effected groups to fill the gap
			for (it++; it != groups.end(); it++)
			{
				(*it->second).rollCounterClockwise(1);
			}
		}

		inline void ComponentStorage<EntityProxy>::removeComponents(GroupIdList groupIdList)
		{
			// For the chuck removal we keep track of the next removal group (nextIt),
			// This allows us to go through effected groups only once, while accumulating
			// the roll ammount, so all groups fill-in the gaps left by removed components.
			int32_t accRoll = 0;
			GroupIdList::iterator beg = groupIdList.begin();
			GroupIt<EntityProxy> nextIt = groups.find(beg->first);
			while (beg != groupIdList.end())
			{
				GroupIt<EntityProxy> it = nextIt;
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

#endif //!__ENTITYSTORAGE__H__