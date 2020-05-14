#ifndef COMPONENTSMANAGER_HPP
#define COMPONENTSMANAGER_HPP

#include "TemplateMaskPack.h"
#include "ComponentStorage.hpp"

using std::tuple;

namespace rv
{
    class ComponentsManager
    {
      private:

        template <class... TComponents> constexpr static intptr_t getTypeMask()
        {
            return MaskPack<TComponents...>::mask();
        }

        template <class... TComponents> constexpr static MaskArray<sizeof...(TComponents)> getMaskArray()
        {
            return {
                reinterpret_cast<intptr_t>(ComponentStorage<TComponents>::getInstance())...
                };
        }

        // Sets offset for the relative ComponentTypes
        template <class TComponent, class... TComponents> static void createComponent(TComponent& arg, MaskArray<sizeof...(TComponents)> maskArray)
        {
            ComponentStorage<TComponent>* storage = ComponentStorage<TComponent>::getInstance();
            storage->addComponent(maskArray.data(), sizeof...(TComponents), &arg, 1);
        }

        template <class TComponent> static ComponentsIterator<TComponent> getComponentIterator(intptr_t mask)
        {
            ComponentStorage<TComponent>* storage = ComponentStorage<TComponent>::getInstance();
            return storage->getComponentIterator(mask);
        }

      public:
        template <class... TComponents> static void createComponents(TComponents... args)
        {
            using expander = int[];
            MaskArray<sizeof...(TComponents)> masks = getMaskArray<TComponents...>();
            expander{0, ((void)(createComponent<TComponents, TComponents...>(args, masks)), 0)...};
        }

        template <class... TComponents> static tuple<ComponentsIterator<TComponents>...> getComponentIterators()
        {
            intptr_t mask = getTypeMask<TComponents...>();
            return {
                getComponentIterator<TComponents>(mask)...
                };
        }
    };

} // namespace rv

#endif