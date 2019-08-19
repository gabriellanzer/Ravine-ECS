#ifndef COMPONENT_LIST_H
#define COMPONENT_LIST_H

#include <unordered_map>
using std::unordered_map;

class ComponentsList
{
	unordered_map<ComponentsList*, size_t*> relativeOffsets;
	const uint32_t dataTypeSize;

public:
	~ComponentsList() = default;
	ComponentsList() : dataTypeSize(UINT32_MAX)
	{

	}

	explicit ComponentsList(const uint32_t dataTypeSize) : dataTypeSize(dataTypeSize)
	{

	}

	size_t* linkRelativeOffset(ComponentsList* relativeList)
	{
		const auto res = relativeOffsets.emplace(relativeList, new size_t(0));
		return res.first->second;
	}

	size_t* getRelativeOffset(ComponentsList* relativeList)
	{
		const auto it = relativeOffsets.find(relativeList);
		if (it != relativeOffsets.end())
		{
			return it->second;
		}
		return nullptr;
	}

	//Increment all offsets starting a given pos
	void incrementRelativeOffset(ComponentsList* relativeList, const size_t startPos, const size_t count = 1)
	{
		const auto it = relativeOffsets.find(relativeList);
		if (it != relativeOffsets.end())
		{
			if (*it->second > startPos)
			{
				*it->second += count;
			}
		}
	}

	//Decrement all offsets starting a given pos
	void decrementRelativeOffset(ComponentsList* relativeList, const size_t startPos, const size_t count = 1)
	{
		const auto it = relativeOffsets.find(relativeList);
		if (it != relativeOffsets.end())
		{
			if (*it->second > startPos)
			{
				*it->second -= count;
			}
		}
	}

};

#endif
