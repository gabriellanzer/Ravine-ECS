#ifndef __ENTITYSTORAGE__H__
#define __ENTITYSTORAGE__H__

#include "ComponentStorage.hpp"
#include "Entity.hpp"

namespace rv
{
	namespace
	{
		class EntityStorage : public ComponentStorage<EntityProxy>
		{
		  public:
			constexpr EntityStorage() : ComponentStorage() {}
			
			void FlushEntityLookup()
			{
				for (GroupCIt<EntityProxy> it = groups.cbegin(); it != groups.cend(); it++)
				{
					
				}
			}
		};
	} // namespace

} // namespace rv

#endif //!__ENTITYSTORAGE__H__