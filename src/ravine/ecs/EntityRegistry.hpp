#ifndef ENTITYREGISTRY_HPP
#define ENTITYREGISTRY_HPP

#include "ComponentStorage.hpp"
#include "ComponentsGroup.hpp"
#include "Entity.hpp"
#include "TemplateMaskPack.h"

#include <algorithm>
#include <queue>
#include <unordered_set>

namespace rv
{
	using std::tuple;
	using std::unordered_set;

	class EntityRegistry
	{
		/**
		 * @brief A base system of any type is a friend class and can use the internal functions.
		 *
		 * @tparam TComponents Types of components.
		 */
		template <class... TComponents>
		friend class BaseSystem;

		/**
		 * @brief Map with a list of entities to be destroyed for each of their archetype groups.
		 */
		static GroupIdList entIdToDestroy;

		/**
		 * @brief Set with all storages whose at least one entity has been removed from.
		 */
		static std::unordered_set<IComponentStorage*> storagesToDestroy;

		/**
		 * @brief Registry of tracked entities.
		 */
		static std::vector<EntityReg> entityRegistry;

		/**
		 * @brief Vacant positions queue on entity registry.
		 */
		static std::queue<Entity> entTableVacancy;

	  public:
		/**
		 * @brief Creates an Entity with the given initialized Components.
		 *
		 * @tparam TComponents Type of the components to store.
		 * @param args Initialized Components to store for this Entity.
		 * @return Entity An entity that is bound to the given components.
		 */
		template <class... TComponents>
		inline static Entity createEntity(TComponents... args);

		/**
		 * @brief Creates an Entity with the given Components Types.
		 *
		 * @tparam TComponents Type of the components to default construct.
		 * @return Entity An entity that is bound to the created components.
		 */
		template <class... TComponents>
		inline static Entity createEntity();

		/**
		 * @brief Removes a given entity immediately.
		 *
		 * @param entity The entity to be removed through immediate operations.
		 */
		inline static void removeEntity(Entity& entity);

		/**
		 * @brief Removes a given entity at the end of the application frame.
		 * Happens uppon calling of the *flushEntityOperations* function.
		 *
		 * @param entity The entity to be removed through batch operations.
		 */
		inline static void lateRemoveEntity(Entity& entity);

		/**
		 * @brief Sorts remove/create entities lists and calls removal operations on the storages.
		 *
		 */
		inline static void flushEntityOperations();

	  private:
		template <class... TComponents>
		constexpr static MaskArray<sizeof...(TComponents)> getMaskArray();

		template <class TComponent>
		inline static CompGroupIt<TComponent> getComponentIterator(intptr_t mask);

		template <class... TComponents>
		inline static tuple<CompGroupIt<TComponents>...> getComponentIterators();

		template <class TComponent, class... TComponents>
		inline static void createComponent(MaskArray<sizeof...(TComponents)> maskArray,
						   const TComponent& arg = TComponent());

		template <class... TComponents>
		inline static void createComponents(MaskArray<sizeof...(TComponents)>& maskArray, TComponents... args);

		template <class EntityReg, class... TComponents>
		inline static void createComponents(MaskArray<sizeof...(TComponents) + 1>& maskArray, EntityReg reg);
	};

	// Static Definitions
	inline GroupIdList EntityRegistry::entIdToDestroy;
	inline std::unordered_set<IComponentStorage*> EntityRegistry::storagesToDestroy;
	inline std::vector<EntityReg> EntityRegistry::entityRegistry;
	inline std::queue<Entity> EntityRegistry::entTableVacancy;

	template <class... TComponents>
	constexpr MaskArray<sizeof...(TComponents)> EntityRegistry::getMaskArray()
	{
		return {reinterpret_cast<intptr_t>(ComponentStorage<TComponents>::getInstance())...};
	}

	template <class TComponent>
	inline CompGroupIt<TComponent> EntityRegistry::getComponentIterator(intptr_t mask)
	{
		ComponentStorage<TComponent>* storage = ComponentStorage<TComponent>::getInstance();
		return storage->getComponentIterator(mask);
	}

	template <class... TComponents>
	inline tuple<CompGroupIt<TComponents>...> EntityRegistry::getComponentIterators()
	{
		intptr_t mask = MaskPack<TComponents...>::mask();
		return {getComponentIterator<TComponents>(mask)...};
	}

	template <class TComponent, class... TComponents>
	inline void EntityRegistry::createComponent(MaskArray<sizeof...(TComponents)> maskArray, const TComponent& arg)
	{
		ComponentStorage<TComponent>* storage = ComponentStorage<TComponent>::getInstance();
		storage->addComponent(maskArray.data(), sizeof...(TComponents), arg);
	}

	template <class... TComponents>
	inline void EntityRegistry::createComponents(MaskArray<sizeof...(TComponents)>& masks, TComponents... args)
	{
		using expander = int[];
		expander{0, ((void)(createComponent<TComponents, TComponents...>(masks, args)), 0)...};
	}

	template <class EntityReg, class... TComponents>
	inline void EntityRegistry::createComponents(MaskArray<sizeof...(TComponents) + 1>& masks, EntityReg reg)
	{
		using expander = int[];
		createComponent<EntityReg, EntityProxy, TComponents...>(masks, reg);
		expander{0, ((void)(createComponent<TComponents, EntityProxy, TComponents...>(masks)), 0)...};
	}

	template <class... TComponents>
	inline Entity EntityRegistry::createEntity(TComponents... args)
	{
		// Fetch Registry Entry
		Entity entity;
		EntityReg* reg;
		if (entTableVacancy.size() > 0)
		{
			entity = entTableVacancy.front();
			entTableVacancy.pop();
			reg = &entityRegistry[entity];
			delete reg->compTypes;
		}
		else
		{
			entity = entityRegistry.size();
			entityRegistry.push_back(EntityReg());
			reg = &entityRegistry[entity];
		}

		// Override Entity Registry
		reg->uniqueId = entity;
		reg->groupId = -1;
		reg->compTypes = new intptr_t[sizeof...(TComponents) + 1];
		reg->typesCount = sizeof...(TComponents) + 1;
		MaskArray<sizeof...(TComponents) + 1> masks = getMaskArray<EntityProxy, TComponents...>();
		memcpy(reg->compTypes, masks.data(), (sizeof...(TComponents) + 1) * sizeof(intptr_t));
		EntityProxy proxy = {entity, -1};
		createComponents<EntityProxy, TComponents...>(masks, proxy, args...);
		return entity;
	}

	template <class... TComponents>
	inline Entity EntityRegistry::createEntity()
	{
		// Fetch Registry Entry
		Entity entity;
		EntityReg* reg;
		if (entTableVacancy.size() > 0)
		{
			entity = entTableVacancy.front();
			entTableVacancy.pop();
			reg = &entityRegistry[entity];
			delete reg->compTypes;
		}
		else
		{
			entity = entityRegistry.size();
			entityRegistry.push_back(EntityReg());
			reg = &entityRegistry[entity];
		}

		// Override Entity Registry
		reg->uniqueId = entity;
		reg->groupId = -1;
		reg->compTypes = new intptr_t[sizeof...(TComponents) + 1];
		reg->typesCount = sizeof...(TComponents) + 1;
		MaskArray<sizeof...(TComponents) + 1> masks = getMaskArray<EntityProxy, TComponents...>();
		memcpy(reg->compTypes, masks.data(), (sizeof...(TComponents) + 1) * sizeof(intptr_t));
		EntityProxy proxy = {entity, -1};
		createComponents<EntityProxy, TComponents...>(masks, proxy);
		return entity;
	}

	inline void EntityRegistry::removeEntity(Entity& entity)
	{
		_ASSERT(entity != InvalidEntity);

		// Fetch Registry Entry
		EntityReg* reg = &entityRegistry[entity];

		// Remove components from storages
		GroupMask typeMask(reg->compTypes, reg->typesCount);
		for (int32_t i = 0; i < reg->typesCount; i++)
		{
			IComponentStorage* storage = reinterpret_cast<IComponentStorage*>(reg->compTypes[i]);
			storage->removeComponent(reg->groupId, typeMask);
		}

		// Set as invalid
		reg->uniqueId = InvalidEntity;
		reg->groupId = -1;
		entity = InvalidEntity;
	}

	inline void EntityRegistry::lateRemoveEntity(Entity& entity)
	{
		_ASSERT(entity != InvalidEntity);
		EntityReg entityReg = entityRegistry[entity];
		GroupMask typeMask(entityReg.compTypes, entityReg.typesCount);

		// Mark all this entity storages for cleanup
		for (int32_t i = 0; i < entityReg.typesCount; i++)
		{
			storagesToDestroy.insert((IComponentStorage*)entityReg.compTypes[i]);
		}

		// Get existing group list or create new one
		std::vector<int32_t>* groupIds;
		GroupIdList::iterator it = entIdToDestroy.lower_bound(typeMask);
		if (it != entIdToDestroy.end() && !(entIdToDestroy.key_comp()(typeMask, it->first)))
		{
			groupIds = it->second;
		}
		else
		{
			groupIds = new std::vector<int32_t>();
			entIdToDestroy.insert(it, GroupIdPair(typeMask, groupIds));
		}

		// Add entity group id to remove group
		groupIds->push_back(entityReg.groupId);

		// Set as invalid
		entityReg.uniqueId = InvalidEntity;
		entityReg.groupId = -1;
		entity = InvalidEntity;
	}

	inline void EntityRegistry::flushEntityOperations()
	{
		// Sorts and make unique entities to destroy on all groups
		for (GroupIdList::iterator it = entIdToDestroy.begin(); it != entIdToDestroy.end(); it++)
		{
			std::vector<int32_t>* idsList = it->second;
			std::sort(idsList->begin(), idsList->end());
			idsList->erase(std::unique(idsList->begin(), idsList->end()), idsList->end());
		}

		// Calls removal of the entities for all related storages
		for (IComponentStorage* storage : storagesToDestroy)
		{
			storage->removeComponents(entIdToDestroy);
		}

		// Cleanup for next frame
		entIdToDestroy.clear();
		storagesToDestroy.clear();

		// TODO Flush late create list
	}

} // namespace rv

#endif