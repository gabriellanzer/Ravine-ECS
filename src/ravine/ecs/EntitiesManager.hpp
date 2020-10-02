#ifndef ENTITIESMANAGER_HPP
#define ENTITIESMANAGER_HPP

#include "ComponentStorage.hpp"
#include "ComponentsGroup.hpp"
#include "Entity.hpp"
#include "TemplateMaskPack.h"
#include <algorithm>
#include <unordered_set>


namespace rv
{
    using std::tuple;
    using std::unordered_set;

    class EntitiesManager
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
         * 
         */
        static GroupIdList entIdToDestroy;

        /**
         * @brief Set with all storages whose at least one entity has been removed from.
         * 
         */
        static std::unordered_set<IComponentStorage*> storagesToDestroy;

        /**
         * @brief Table of positions for unique entity id.
         * 
         */
        static std::vector<Entity*> entitiesTable;

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
        inline static TComponent* createComponent(MaskArray<sizeof...(TComponents)> maskArray,
                                                  const TComponent& arg = TComponent());

        template <class... TComponents>
        inline static Entity* createComponents(TComponents... args);

        template <class... TComponents>
        inline static Entity* createComponents();
    };

    // Static Definitions
    GroupIdList EntitiesManager::entIdToDestroy;
    std::unordered_set<IComponentStorage*> EntitiesManager::storagesToDestroy;

    template <class... TComponents>
    constexpr MaskArray<sizeof...(TComponents)> EntitiesManager::getMaskArray()
    {
        return {reinterpret_cast<intptr_t>(ComponentStorage<TComponents>::getInstance())...};
    }

    template <class TComponent>
    inline CompGroupIt<TComponent> EntitiesManager::getComponentIterator(intptr_t mask)
    {
        ComponentStorage<TComponent>* storage = ComponentStorage<TComponent>::getInstance();
        return storage->getComponentIterator(mask);
    }

    template <class... TComponents>
    inline tuple<CompGroupIt<TComponents>...> EntitiesManager::getComponentIterators()
    {
        intptr_t mask = MaskPack<TComponents...>::mask();
        return {getComponentIterator<TComponents>(mask)...};
    }

    template <class TComponent, class... TComponents>
    inline TComponent* EntitiesManager::createComponent(MaskArray<sizeof...(TComponents)> maskArray,
                                                        const TComponent& arg)
    {
        ComponentStorage<TComponent>* storage = ComponentStorage<TComponent>::getInstance();
        return storage->addComponent(maskArray.data(), sizeof...(TComponents), arg);
    }

    template <class... TComponents>
    inline Entity* EntitiesManager::createComponents(TComponents... args)
    {
        using expander = int[];
        MaskArray<sizeof...(TComponents) + 1> masks = getMaskArray<Entity, TComponents...>();
        Entity* entity = createComponent<Entity, Entity, TComponents...>(masks);
        entity->compTypes = new intptr_t[sizeof...(TComponents) + 1];
        memcpy(entity->compTypes, masks.data(), (sizeof...(TComponents) + 1) * sizeof(intptr_t));
        entity->typesCount = sizeof...(TComponents) + 1;
        expander{0, ((void)(createComponent<TComponents, Entity, TComponents...>(masks, args)), 0)...};
        return entity;
    }

    template <class... TComponents>
    inline Entity* EntitiesManager::createComponents()
    {
        using expander = int[];
        MaskArray<sizeof...(TComponents) + 1> masks = getMaskArray<Entity, TComponents...>();
        Entity* entity = createComponent<Entity, Entity, TComponents...>(masks);
        entity->compTypes = new intptr_t[sizeof...(TComponents) + 1];
        memcpy(entity->compTypes, masks.data(), (sizeof...(TComponents) + 1) * sizeof(intptr_t));
        entity->typesCount = sizeof...(TComponents) + 1;
        expander{0, ((void)(createComponent<TComponents, Entity, TComponents...>(masks)), 0)...};
        return entity;
    }

    template <class... TComponents>
    inline Entity EntitiesManager::createEntity(TComponents... args)
    {
        return *createComponents<TComponents...>(args...);
    }

    template <class... TComponents>
    inline Entity EntitiesManager::createEntity()
    {
        return *createComponents<TComponents...>();
    }

    inline void EntitiesManager::removeEntity(Entity& entity)
    {
        _ASSERT(entity.id != -1);
        GroupMask typeMask(entity.compTypes, entity.typesCount);
        for (int32_t i = 0; i < entity.typesCount; i++)
        {
            IComponentStorage* storage = reinterpret_cast<IComponentStorage*>(entity.compTypes[i]);
            storage->removeComponent(entity.groupId, typeMask);
        }
        // Set as invalid
        entity.id = -1;
        entity.groupId = -1;
    }

    inline void EntitiesManager::lateRemoveEntity(Entity& entity)
    {
        _ASSERT(entity.id != -1);
        GroupMask typeMask(entity.compTypes, entity.typesCount);

        // Mark all this entity storages for cleanup
        for (int32_t i = 0; i < entity.typesCount; i++)
        {
            storagesToDestroy.insert((IComponentStorage*)entity.compTypes[i]);
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

        // Add entity to remove group
        groupIds->push_back(entity.id);

        // Set as invalid
        entity.id = -1;
        entity.groupId = -1;
    }

    inline void EntitiesManager::flushEntityOperations()
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