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
        constexpr static intptr_t getTypeMask()
        {
            return MaskPack<TComponents...>::mask();
        }

        template <class... TComponents>
        constexpr static MaskArray<sizeof...(TComponents)> getMaskArray()
        {
            return {reinterpret_cast<intptr_t>(ComponentStorage<TComponents>::getInstance())...};
        }

        template <class TComponent>
        inline static CompGroupIt<TComponent> getComponentIterator(intptr_t mask)
        {
            ComponentStorage<TComponent>* storage = ComponentStorage<TComponent>::getInstance();
            return storage->getComponentIterator(mask);
        }

        template <class... TComponents>
        inline static tuple<CompGroupIt<TComponents>...> getComponentIterators()
        {
            intptr_t mask = getTypeMask<TComponents...>();
            return {getComponentIterator<TComponents>(mask)...};
        }

        template <class TComponent, class... TComponents>
        inline static TComponent& createComponent(MaskArray<sizeof...(TComponents)> maskArray,
                                                  const TComponent& arg = TComponent())
        {
            ComponentStorage<TComponent>* storage = ComponentStorage<TComponent>::getInstance();
            return storage->addComponent(maskArray.data(), sizeof...(TComponents), arg);
        }

        template <class... TComponents>
        inline static Entity& createComponents(const TComponents&... args)
        {
            using expander = int[];
            MaskArray<sizeof...(TComponents) + 1> masks = getMaskArray<Entity, TComponents...>();
            Entity& entity = createComponent<Entity, Entity, TComponents...>(masks);
            expander{0, ((void)(createComponent<TComponents, Entity, TComponents...>(masks, args)), 0)...};
            return entity;
        }

        template <class... TComponents>
        inline static Entity& createComponents()
        {
            using expander = int[];
            MaskArray<sizeof...(TComponents) + 1> masks = getMaskArray<Entity, TComponents...>();
            Entity& entity = createComponent<Entity, Entity, TComponents...>(masks);
            expander{0, ((void)(createComponent<TComponents, Entity, TComponents...>(masks)), 0)...};
            return entity;
        }

      public:
        template <typename... TComps>
        inline static Entity& createEntity(TComps... args)
        {
            return EntitiesManager::createComponents<TComps...>(args...);
        }

        template <typename... TComps>
        inline static Entity& createEntity()
        {
            return EntitiesManager::createComponents<TComps...>();
        }
    };

} // namespace rv

#endif