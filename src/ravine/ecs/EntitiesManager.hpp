#ifndef ENTITIESMANAGER_HPP
#define ENTITIESMANAGER_HPP

#include "ComponentStorage.hpp"
#include "Entity.hpp"
#include "TemplateMaskPack.h"

using std::tuple;

namespace rv
{

    class EntitiesManager
    {
        template <class... TComponents>
        friend class BaseSystem;

      private:
        template <class... TComponents>
        constexpr static intptr_t getTypeMask();

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

      public:
        template <class... TComponents>
        inline static Entity createEntity(TComponents... args);

        template <class... TComponents>
        inline static Entity createEntity();

        template <class... TComponents>
        inline static void removeEntity(Entity& entity);
    };

    template <class... TComponents>
    constexpr intptr_t EntitiesManager::getTypeMask()
    {
        return MaskPack<TComponents...>::mask();
    }

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
        intptr_t mask = getTypeMask<TComponents...>();
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

    template <class... TComponents>
    inline void EntitiesManager::removeEntity(Entity& entity)
    {
        _ASSERT(entity.id != -1);
        GroupMask typeMask(entity.compTypes, entity.typesCount);
        for (int32_t i = 0; i < entity.typesCount; i++)
        {
            IComponentStorage* storage = reinterpret_cast<IComponentStorage*>(entity.compTypes[i]);
            storage->removeComponent(entity.id, typeMask);
        }
        // Set as invalid
        entity.id = -1;
    }

} // namespace rv

#endif