#ifndef ICOMPONENTSTORAGE_HPP
# define ICOMPONENTSTORAGE_HPP

#include <inttypes.h>

namespace rv
{
    class IComponentStorage
    {
        public:
        IComponentStorage() = default;
        virtual ~IComponentStorage() = default;
        virtual inline void swapComponent(int32_t entityId, intptr_t* types, int32_t typesCount) = 0;
        virtual inline void removeComponent(int32_t entityId, intptr_t* types, int32_t typesCount) = 0;
    };
} // namespace rv

#endif