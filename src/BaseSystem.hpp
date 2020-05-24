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
        tuple<TComponents*...> chunkData;

        template <int... T> struct FetchPack;

        template <> struct FetchPack<>
        {
            static constexpr intptr_t fetchChunk(tuple<TComponents*...>& chunkData,
                                                 tuple<CompGroupIt<TComponents>...>& compIt, int32_t groupId,
                                                 int32_t fetchId)
            {
                return INT32_MAX;
            }
        };

        template <int I, int... S> struct FetchPack<I, S...>
        {
            static constexpr int32_t fetchChunk(tuple<TComponents*...>& chunkData,
                                                tuple<CompGroupIt<TComponents>...>& compIt, int32_t groupId,
                                                int32_t fetchId)
            {
                int32_t lGroupSize = 0;
                get<I>(chunkData) = get<I>(compIt).compIt[groupId].getChunk(fetchId, lGroupSize);
                int32_t rGroupSize = FetchPack<S...>::fetchChunk(chunkData, compIt, groupId, fetchId);
                return (lGroupSize < rGroupSize) ? lGroupSize : rGroupSize;
            }
        };

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
            const uint8_t groupCount = get<0>(compIterators).count;
            for (uint8_t i = 0; i < groupCount; i++)
            {
                int32_t fetchIt = 0;
                int32_t groupSize = get<0>(compIterators).compIt[i].getSize();
                while (fetchIt < groupSize)
                {
                    int32_t chunkSize = FetchPack<S...>::fetchChunk(chunkData, compIterators, i, fetchIt);
                    update(deltaTime, chunkSize, get<S>(chunkData)...);
                    fetchIt += chunkSize;
                }
            }
        }

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
        virtual void update(double deltaTime, int32_t size, TComponents* const... components) = 0;
    };

} // namespace rv

#endif