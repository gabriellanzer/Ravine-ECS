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
	static ComponentsList* getOffsetsList()
	{
		static ComponentsList offsetsList(sizeof(TComponent));
		return &offsetsList;
	}

	//Sets offset for the relative ComponentTypes
	template<class TComponent, class TOtherComponent>
	static size_t* createOffset()
	{
		size_t* offset = getRelativeOffset<TComponent, TOtherComponent>();

		//No offset found
		if (offset == nullptr)
		{
			ComponentsList* offsetsList = getOffsetsList<TComponent>();
			offset = offsetsList->linkRelativeOffset(getOffsetsList<TOtherComponent>());

			//Unless of same type, offset is current array size
			if (static_cast<bool>(!std::is_same<TComponent, TOtherComponent>::value))
			{
				*offset = getArray<TComponent>(0).size();
			}
		}

		return offset;
	}

	//Sets offset for the relative ComponentTypes
	template<class TComponent, class... TComponents>
	static array<size_t*, sizeof...(TComponents)> createComponentOffset()
	{
		return {
			createOffset<TComponent, TComponents>()...
		};
	}

	//Unique offset for each type combination
	template<class TComponent, class TOtherComponent>
	static size_t* getRelativeOffset()
	{
		ComponentsList* list = getOffsetsList<TComponent>();
		ComponentsList* otherList = getOffsetsList<TOtherComponent>();
		return list->getRelativeOffset(otherList);
	}

	//Final Getting call
	template<class TComponent>
	static size_t* getComponentOffset()
	{
		ComponentsList* list = getOffsetsList<TComponent>();
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

	//Final Getting call
	template<class TComponent>
	static size_t getComponentSize()
	{
		return getArray<TComponent>(0).size();
	}

	//Chosing correct size from pair's comparation
	template<class TComponent, class TOtherComponent, class... TComponents>
	static size_t getComponentSize()
	{
		//Ignore size with same type unless it is the only component
		if (static_cast<bool>(std::is_same<TComponent, TOtherComponent>::value))
		{
			//Only this component left
			if (sizeof...(TComponents) == 0)
			{
				return getArray<TComponent>().size();
			}

			return getComponentSize<TComponent, TComponents...>();
		}
		vector<TComponent>& arr = getArray<TComponent>();
		const size_t size = arr.size() - *getRelativeOffset<TComponent, TOtherComponent>();
		const size_t otherSize = getComponentSize<TComponent, TComponents...>();
		if (otherSize < size)
			return otherSize;
		return size;
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
			ComponentsList* list = getOffsetsList<TComponent>();
			ComponentsList* otherList = getOffsetsList<TOtherComponent>();
			list->incrementRelativeOffset(otherList, startPos, count);
		}
		increaseOffsets<TComponent, TComponents...>(startPos, count);
	}

	//Sets offset for the relative ComponentTypes
	template<class TComponent, class... TComponents>
	static void createComponent(TComponent& arg)
	{
		createComponentOffset<TComponent, TComponents...>();
		size_t* offset = getComponentOffset<TComponent, TComponents...>();
		increaseOffsets<TComponent, TComponents...>(*offset);
		vector<TComponent>& compArray = getArray<TComponent>();
		compArray.insert(compArray.begin() + *offset, arg);
	}

public:
	template<class TComponent>
	static vector<TComponent>& getArray(size_t allocSize = 1024)
	{
		static vector<TComponent> compList;
		compList.reserve(1000);
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
			getComponentSize<TComponents, TComponents...>()...
		};
	}
};