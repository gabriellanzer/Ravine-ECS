#pragma once
#include <tuple>
#include "TemplateIndexPack.h"
#include "ComponentsManager.hpp"
#include "ISystem.h"

using std::tuple;
using std::get;

template<class... TComponents>
class BaseSystem : public ISystem
{
private:
	tuple<TComponents*...> componentLists;

	template<int it>
	constexpr void getComponent(tuple<TComponents*...>& componentLists, array<size_t, sizeof...(TComponents)> offsets)
	{

	}

	template<int it, class TComponent, class... TArgs>
	constexpr void getComponent(tuple<TComponents*...>& componentLists, array<size_t, sizeof...(TComponents)> offsets)
	{
		//Get Component List from manager for TComponent type
		vector<TComponent>& componentList = ComponentsManager::getArray<TComponent>();
		get<it>(componentLists) = componentList.data() + get<it>(offsets);

		getComponent<it + 1, TArgs...>(componentLists, offsets);
	}

	template<int... S>
	constexpr void updateUnfold(double deltaTime, size_t size, tuple<TComponents*...>& componentLists, seq<S...>)
	{
		update(deltaTime, size, get<S>(componentLists)...);
	}

	template<int... S>
	constexpr void updateById(size_t id, double deltaTime, tuple<TComponents* ...>& componentLists, seq<S...>)
	{
		VirtualUpdate(deltaTime, get<S>(componentLists)[id]...);
	}

public:
	// Herdado por meio de ISystem
	virtual void update(double deltaTime) final
	{
		//Get Updated List of components
		auto offsets = ComponentsManager::getOffsets<TComponents...>();
		getComponent<0, TComponents...>(componentLists, offsets);
		size_t size = (size_t)-1;
		auto sizes = ComponentsManager::getSizes<TComponents...>();
		for (size_t it : sizes)
		{
			if (it < size)
				size = it;
		}

		updateUnfold(deltaTime, size, componentLists, typename gens<sizeof...(TComponents)>::type());
	}

	virtual void update(double deltaTime, size_t size, TComponents*... components) = 0;

};