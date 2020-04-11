#ifndef BASESYSTEM_HPP
#define BASESYSTEM_HPP

#include "ComponentsManager.hpp"
#include "ISystem.h"
#include "TemplateIndexPack.h"
#include <tuple>

using std::get;
using std::tuple;

namespace rv
{

template <class... TComponents> class BaseSystem : public ISystem
{
  private:
    tuple<TComponents *...> componentLists;

    template <int It>
    constexpr void getComponent(tuple<TComponents *...> &componentLists, array<size_t, sizeof...(TComponents)> offsets)
    {
    }

    template <int It, class TComponent, class... TArgs>
    constexpr void getComponent(tuple<TComponents *...> &componentLists, array<size_t, sizeof...(TComponents)> offsets)
    {
        // Get Component List from manager for TComponent type
        vector<TComponent> &componentList = ComponentsManager::getArray<TComponent>();
        get<It>(componentLists) = componentList.data() + get<It>(offsets);

        getComponent<It + 1, TArgs...>(componentLists, offsets);
    }

    /**
     * @brief Calls the virtual \see{update} function by unfolding their arguments with a compile-time sequence list.
     *
     * @tparam S Type list id sequence
     * @param deltaTime Time since last update
     * @param size Amount of entities the components represent
     * @param componentLists Lists for each component type this system runs through
     */
    template <int... S>
    constexpr void updateUnfold(double deltaTime, size_t size, tuple<TComponents *...> &componentLists, seq<S...>)
    {
        update(deltaTime, size, get<S>(componentLists)...);
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
    void update(double deltaTime) override final
    {
        // Get Updated List of components
        auto offsets = ComponentsManager::getOffsets<TComponents...>();
        getComponent<0, TComponents...>(componentLists, offsets);
        size_t size = static_cast<size_t>(-1);
        auto sizes = ComponentsManager::getSizes<TComponents...>();
        for (size_t it : sizes)
        {
            if (it < size)
                size = it;
        }

        updateUnfold(deltaTime, size, componentLists, typename gens<sizeof...(TComponents)>::type());
    }

    /**
     * @brief Update virtual function to be overriten by a System implementation.
     *  Called by the \see{BaseSystem} class through \see{SystemManager} command.
     *
     * @param deltaTime Timespan between last and current frame (in seconds).
     * @param size Amount of entities the components represent.
     * @param components List expansion for each component type this system runs through.
     */
    virtual void update(double deltaTime, size_t size, TComponents *... components) = 0;
};

} // namespace rv

#endif