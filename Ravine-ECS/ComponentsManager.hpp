#pragma once
#include <vector>
#include <array>
#include <type_traits>
#include <unordered_set>
#include "TemplateIndexPack.h"
#include "ComponentsList.hpp"

using std::vector;
using std::array;
using std::tuple;
using std::pair;
using std::unordered_set;

class ComponentsManager
{
private:
	template<class TComponent>
	static ComponentsList* getRelativityList()
	{
		static ComponentsList offsetsList(sizeof(TComponent));
		return &offsetsList;
	}

	//Sets offset for the relative ComponentTypes
	template<class TComponent, class TOtherComponent>
	static size_t* createRelation()
	{
		size_t* offset = getRelativeOffset<TComponent, TOtherComponent>();

		//No offset found
		if (offset == nullptr)
		{
			ComponentsList* relativityList = getRelativityList<TComponent>();
			offset = relativityList->linkRelativeOffset(getRelativityList<TOtherComponent>());

			//Unless of same type, offset is current array size
			if (static_cast<bool>(!std::is_same<TComponent, TOtherComponent>::value))
			{
				*offset = getArray<TComponent>(0).size();
			}
		}

		size_t* size = getRelativeSize<TComponent, TOtherComponent>();

		//No size found
		if (size == nullptr)
		{
			ComponentsList* relativityList = getRelativityList<TComponent>();
			size = relativityList->linkRelativeSize(getRelativityList<TOtherComponent>());
		}

		return offset;
	}

	//Sets offset for the relative ComponentTypes
	template<class TComponent, class... TComponents>
	static array<size_t*, sizeof...(TComponents)> createComponentRelation()
	{
		return {
			createRelation<TComponent, TComponents>()...
		};
	}

	//Unique offset for each type combination
	template<class TComponent, class TOtherComponent>
	static size_t* getRelativeOffset()
	{
		ComponentsList* list = getRelativityList<TComponent>();
		ComponentsList* otherList = getRelativityList<TOtherComponent>();
		return list->getRelativeOffset(otherList);
	}

	//Final Getting call
	template<class TComponent>
	static size_t* getComponentOffset()
	{
		ComponentsList* list = getRelativityList<TComponent>();
		return list->getRelativeOffset(list);
	}

	//Choosing correct offset from pair's comparision
	template<class TComponent, class TOtherComponent, class... TComponents>
	static size_t* getComponentOffset()
	{
		//Ignore offset with same type
		if (static_cast<bool>(std::is_same<TComponent, TOtherComponent>::value))
		{
			//Only this component left
			if (sizeof...(TComponents) == 0)
			{
				return getComponentOffset<TComponent>();
			}

			return getComponentOffset<TComponent, TComponents...>();
		}
		size_t* offset = getRelativeOffset<TComponent, TOtherComponent>();
		size_t* otherOffset = getComponentOffset<TComponent, TComponents...>();
		if (*otherOffset > * offset)
			return otherOffset;
		return offset;
	}

	template<class TComponent>
	static void increaseOffsets(const size_t startPos, const size_t count = 1)
	{

	}

	template<class TComponent, class TOtherComponent, class... TComponents>
	static void increaseOffsets(const size_t startPos, const size_t count = 1)
	{
		if (!static_cast<bool>(std::is_same<TComponent, TOtherComponent>::value))
		{
			ComponentsList* list = getRelativityList<TComponent>();
			ComponentsList* otherList = getRelativityList<TOtherComponent>();
			list->incrementRelativeOffset(otherList, startPos, count);
		}
		increaseOffsets<TComponent, TComponents...>(startPos, count);
	}

	//Unique size for each type combination
	template<class TComponent, class TOtherComponent>
	static size_t* getRelativeSize()
	{
		ComponentsList* list = getRelativityList<TComponent>();
		ComponentsList* otherList = getRelativityList<TOtherComponent>();
		return list->getRelativeSize(otherList);
	}
	
	//Final Getting call
	template<class TComponent>
	static size_t* getComponentSize()
	{
		ComponentsList* list = getRelativityList<TComponent>();
		return list->getRelativeSize(list);
	}

	//Choosing correct size from pair's comparision
	template<class TComponent, class TOtherComponent, class... TComponents>
	static size_t* getComponentSize()
	{
		//Ignore size with same type unless it is the only component
		if (static_cast<bool>(std::is_same<TComponent, TOtherComponent>::value))
		{
			//Only this component left
			if (sizeof...(TComponents) == 0)
			{
				return getComponentSize<TComponent>();
			}

			return getComponentSize<TComponent, TComponents...>();
		}
		size_t* size = getRelativeSize<TComponent, TOtherComponent>();
		size_t* otherSize = getComponentSize<TComponent, TComponents...>();
		if (otherSize[0] < size[0])
			return otherSize;
		return size;
	}

	template<class TComponent>
	static void increaseSizes(const size_t startPos = 0, const size_t count = 1)
	{

	}

	template<class TComponent, class TOtherComponent, class... TComponents>
	static void increaseSizes(const size_t startPos = 0, const size_t count = 1)
	{
		ComponentsList* list = getRelativityList<TComponent>();
		ComponentsList* otherList = getRelativityList<TOtherComponent>();
		list->incrementRelativeSize(otherList, startPos, count);
		increaseSizes<TComponent, TComponents...>(startPos, count);
	}

	//Sets offset for the relative ComponentTypes
	template<class TComponent, class... TComponents>
	static void createComponent(TComponent& arg)
	{
		createComponentRelation<TComponent, TComponents...>();
		size_t* offset = getComponentOffset<TComponent, TComponents...>();
		increaseOffsets<TComponent, TComponents...>(*offset);
		increaseSizes<TComponent, TComponents...>();
		vector<TComponent>& compArray = getArray<TComponent>();
		compArray.insert(compArray.begin() + *offset, arg);
	}

public:
	template<class TComponent>
	static vector<TComponent>& getArray(size_t allocSize = 1024)
	{
		static vector<TComponent> compList;
		compList.reserve(allocSize);
		return compList;
	}

	template<class... TComponents>
	static void createComponents(TComponents... args)
	{
		using expander = int[];
		expander{
			0,
			((void)(createComponent<TComponents, TComponents...>(args)), 0)
			...
		};
	}

	template<class... TComponents>
	static array<size_t, sizeof...(TComponents)> getOffsets()
	{
		return {
			*getComponentOffset<TComponents, TComponents...>()...
		};
	}

	template<class... TComponents>
	static array<size_t, sizeof...(TComponents)> getSizes()
	{
		return {
			*getComponentSize<TComponents, TComponents...>()...
		};
	}
};