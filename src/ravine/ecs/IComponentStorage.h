#ifndef ICOMPONENTSTORAGE_HPP
#define ICOMPONENTSTORAGE_HPP

#include "ComponentsGroup.hpp"
#include <inttypes.h>
#include <map>
#include <vector>

namespace rv
{
	using GroupIdList = std::map<GroupMask, std::vector<int32_t>*, GroupMaskCmp>;
	using GroupIdPair = std::pair<GroupMask, std::vector<int32_t>*>;

	class IComponentStorage
	{
	  public:
		IComponentStorage() = default;
		virtual ~IComponentStorage() = default;
		virtual inline void swapComponent(int32_t entityId, GroupMask oldTypeMask, GroupMask newTypeMask) = 0;
		virtual inline void removeComponent(int32_t entityId, GroupMask typeMask) = 0;
		virtual inline void removeComponents(GroupIdList groupIdList) = 0;
	};
} // namespace rv

#endif