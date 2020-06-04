#ifndef ICOMPONENTSTORAGE_HPP
# define ICOMPONENTSTORAGE_HPP

#include <inttypes.h>
#include "ComponentsGroup.hpp"

namespace rv
{
    class IComponentStorage
    {
        public:
        IComponentStorage() = default;
        virtual ~IComponentStorage() = default;
        virtual inline void swapComponent(int32_t entityId, GroupMask oldTypeMask, GroupMask newTypeMask) = 0;
        virtual inline void removeComponent(int32_t entityId, GroupMask typeMask) = 0;
    };
} // namespace rv

#endif