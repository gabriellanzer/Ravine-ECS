#ifndef ENTITYREGISTRY_HPP
#define ENTITYREGISTRY_HPP

#include "Entity.hpp"
#include "EntityStorage.hpp"
#include "TemplateMaskPack.h"
#include "EntityGroup.hpp"
#include "../Time.h"

#include <algorithm>
#include <queue>
#include <unordered_set>
#include <vector>

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
		 * @brief Creates a component Entity for the given type.
		 *
		 * @tparam TComponent Type of the components to create it's entity handle.
		 * @return Entity An entity that is bound to the created component.
		 */
		template <class TComponent>
		inline static Entity createComponentEntity();

		/**
		 * @brief Removes a given entity immediately.
		 *
		 * @param entity The entity to be removed through immediate operations.
		 */
		inline static void removeEntityImediatelly(Entity& entity);

		/**
		 * @brief Removes a given entity at the end of the application frame.
		 * Happens uppon calling of the *flushEntityOperations* function.
		 *
		 * @param entity The entity to be removed through batch operations.
		 */
		inline static void removeEntity(Entity& entity);

		/**
		 * @brief Sorts remove/create entities lists and calls removal operations on the storages.
		 *
		 */
		inline static void flushEntityOperations();

		inline static void patchEntitiesLookup(const std::vector<EntityLookup>& lookupBuf);

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
		expander{0, ((void)(createComponent<TComponents, TComponents...>(masks, args)
		    ), 0)...};
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
		ScopedTimer<TimerMilisseconds, "Test"> timer;

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
		timer.CaptureSample();

		// Override Entity Registry
		reg->entityId = entity;
		reg->groupPos = -1;
		reg->compTypes = new intptr_t[sizeof...(TComponents) + 1];
		reg->typesCount = sizeof...(TComponents) + 1;
		MaskArray<sizeof...(TComponents) + 1> masks = getMaskArray<EntityProxy, TComponents...>();
		memcpy(reg->compTypes, masks.data(), (sizeof...(TComponents) + 1) * sizeof(intptr_t));
		timer.CaptureSample();

		EntityProxy proxy = {entity, -1};
		createComponents<EntityProxy, TComponents...>(masks, proxy, args...);
		timer.CaptureSample();

		// Flush the Entity Proxy storage so we can get updated group positions
		ComponentStorage<EntityProxy>* storage = ComponentStorage<EntityProxy>::getInstance();
		storage->flushEntityLookups(&EntityRegistry::patchEntitiesLookup);
		timer.CaptureSample();

		return entity;
	}

	template <class... TComponents>
	inline Entity EntityRegistry::createEntity()
	{
		ScopedTimer<TimerMilisseconds> timer("");

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
		timer.CaptureSample();

		// Override Entity Registry
		reg->entityId = entity;
		reg->groupPos = -1;
		reg->compTypes = new intptr_t[sizeof...(TComponents) + 1];
		reg->typesCount = sizeof...(TComponents) + 1;
		MaskArray<sizeof...(TComponents) + 1> masks = getMaskArray<EntityProxy, TComponents...>();
		memcpy(reg->compTypes, masks.data(), (sizeof...(TComponents) + 1) * sizeof(intptr_t));
		timer.CaptureSample();
		
		EntityProxy proxy = {entity, -1};
		createComponents<EntityProxy, TComponents...>(masks, proxy);
		timer.CaptureSample();

		// Flush the Entity Proxy storage so we can get updated group positions
		ComponentStorage<EntityProxy>* storage = ComponentStorage<EntityProxy>::getInstance();
		storage->flushEntityLookups(&EntityRegistry::patchEntitiesLookup);
		timer.CaptureSample();

		return entity;
	}

	template <class TComponent>
	inline Entity EntityRegistry::createComponentEntity()
	{
		// Fetch Registry Entry
		static Entity componentEntity = InvalidEntity;
		if (componentEntity != InvalidEntity) return componentEntity;

		EntityReg* reg;
		if (entTableVacancy.size() > 0)
		{
			componentEntity = entTableVacancy.front();
			entTableVacancy.pop();
			reg = &entityRegistry[componentEntity];
			delete reg->compTypes;
		}
		else
		{
			componentEntity = entityRegistry.size();
			entityRegistry.push_back(EntityReg());
			reg = &entityRegistry[componentEntity];
		}

		// TODO: Create meta-components here
		reg->entityId = componentEntity;
		reg->groupPos = -1;
		reg->compTypes = new intptr_t[0];
		reg->typesCount = 0;

		return componentEntity;
	}

	inline void EntityRegistry::removeEntityImediatelly(Entity& entity)
	{
		_ASSERT(entity != InvalidEntity);

		// Fetch Registry Entry
		EntityReg* reg = &entityRegistry[entity];

		// Remove components from storages
		GroupMask typeMask(reg->compTypes, reg->typesCount);
		for (int32_t i = 0; i < reg->typesCount; i++)
		{
			IComponentStorage* storage = reinterpret_cast<IComponentStorage*>(reg->compTypes[i]);
			storage->removeComponent(reg->groupPos, typeMask);
		}

		// Flush the Entity Proxy storage so we can get updated group positions
		ComponentStorage<EntityProxy>* storage = ComponentStorage<EntityProxy>::getInstance();
		storage->flushEntityLookups(&EntityRegistry::patchEntitiesLookup);

		// Open up an entity registry slot
		entTableVacancy.push(entity);

		// Set as invalid
		reg->entityId = InvalidEntity;
		reg->groupPos = -1;
		entity = InvalidEntity;
	}

	inline void EntityRegistry::removeEntity(Entity& entity)
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
		std::vector<int32_t>* groupPosToRmv;
		GroupIdList::iterator it = entIdToDestroy.lower_bound(typeMask);
		if (it != entIdToDestroy.end() && !(entIdToDestroy.key_comp()(typeMask, it->first)))
		{
			groupPosToRmv = it->second;
		}
		else
		{
			groupPosToRmv = new std::vector<int32_t>();
			entIdToDestroy.insert(it, GroupIdPair(typeMask, groupPosToRmv));
		}

		// Add entity group id to remove group
		groupPosToRmv->push_back(entityReg.groupPos);

		// Open up an entity registry slot
		entTableVacancy.push(entity);

		// Set as invalid
		entityReg.entityId = InvalidEntity;
		entityReg.groupPos = -1;
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

		// Flush the Entity Proxy storage so we can get updated group positions
		ComponentStorage<EntityProxy>* storage = ComponentStorage<EntityProxy>::getInstance();
		storage->flushEntityLookups(&EntityRegistry::patchEntitiesLookup);

		// Cleanup for next frame
		entIdToDestroy.clear();
		storagesToDestroy.clear();

		// TODO Flush late create list
	}

	inline void EntityRegistry::patchEntitiesLookup(const std::vector<EntityLookup>& lookupBuf)
	{
		for (const EntityLookup& lookup : lookupBuf)
		{
			EntityReg& reg = entityRegistry[lookup.entityId];
			reg.groupPos = lookup.groupPos;
		}
	}

} // namespace rv

#endif