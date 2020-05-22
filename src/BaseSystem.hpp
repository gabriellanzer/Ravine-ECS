#ifndef BASESYSTEM_HPP
#define BASESYSTEM_HPP

#include "ComponentsManager.hpp"
#include "ISystem.h"
#include "TemplateIndexPack.h"

using std::get;

namespace rv
{

    template <class... TComponents> class BaseSystem : public ISystem
    {
      private:
        tuple<CompGroupIt<TComponents>...> compIterators;

        /**
         * @brief Calls the virtual \see{update} function by unfolding their arguments with a compile-time sequence
         * list.
         *
         * @tparam S Type list id sequence
         * @param deltaTime Time since last update
         * @param componentIt Iterator for each component type this system runs through
         */
        template <int... S> constexpr void updateUnfold(double deltaTime, seq<S...>)
        {
            uint8_t groupCount = get<0>(compIterators).count;
            for (uint8_t i = 0; i < groupCount; i++)
            {
                int32_t groupSize = get<0>(compIterators).sizes[i];
                update(deltaTime, groupSize, get<S>(compIterators).offsets[i]...);
            }
        }

        // This function was a test using a for-loop in BaseSystem class calling a virtual update function that
        // passes on component references instead of pointer and size, nice test case of virtual function
        // overhead (MUCH SLOWER!)
        // template<int... S>
        // constexpr void updateById(size_t id, double deltaTime, tuple<TComponents* ...>& componentLists, seq<S...>)
        //{
        //	update(deltaTime, get<S>(componentLists)[id]...);
        //}

      public:
        /**
         * @brief Update base function, called by the ECS framework \see{SystemManager}.
         *
         * @param deltaTime Timespan between last and current frame (in seconds).
         */
        void update(double deltaTime) final
        {
            // Get Updated List of Iterators
            compIterators = ComponentsManager::getComponentIterators<TComponents...>();
            updateUnfold(deltaTime, typename gens<sizeof...(TComponents)>::type());
        }

        /**
         * @brief Update virtual function to be overriten by a System implementation.
         *  Called by the \see{BaseSystem} class through \see{SystemManager} command.
         *
         * @param deltaTime Timespan between last and current frame (in seconds).
         * @param size Amount of entities the components represent.
         * @param components List expansion for each component type this system runs through.
         */
        virtual void update(double deltaTime, size_t size, TComponents* const... components) = 0;
    };

} // namespace rv

#endif