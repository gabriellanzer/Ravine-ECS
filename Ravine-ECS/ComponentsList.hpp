#ifndef COMPONENT_LIST_H
#define COMPONENT_LIST_H

#include <unordered_map>
using std::unordered_map;

class ComponentsList
{
	unordered_map<ComponentsList*, size_t*> relativeOffsets;
	unordered_map<ComponentsList*, size_t*> relativeSizes;
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

	size_t* linkRelativeSize(ComponentsList* relativeList)
	{
		const auto res = relativeSizes.emplace(relativeList, new size_t(0));
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

	size_t* getRelativeSize(ComponentsList* relativeList)
	{
		const auto it = relativeSizes.find(relativeList);
		if (it != relativeSizes.end())
		{
			return it->second;
		}
		return nullptr;
	}

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

	void incrementRelativeOffsets(const size_t startPos, const size_t count = 1)
	{
		for (const auto it : relativeOffsets)
		{
			if (*it.second > startPos)
			{
				*it.second += count;
			}
		}
	}

	void incrementRelativeSize(ComponentsList* relativeList, const size_t startPos, const size_t count = 1)
	{
		const auto itOffset = relativeOffsets.find(relativeList);
		const auto itSize = relativeSizes.find(relativeList);
		if (itOffset != relativeOffsets.end())
		{
			if (*itOffset->second >= startPos)
			{
				*itSize->second += count;
			}
		}
	}

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

	void decreaseRelativeOffsets(const size_t startPos, const size_t count = 1)
	{
		for (const auto it : relativeOffsets)
		{
			if (*it.second > startPos)
			{
				*it.second -= count;
			}
		}
	}

	void decrementRelativeSizes(ComponentsList* relativeList, const size_t startPos, const size_t count = 1)
	{
		const auto it = relativeSizes.find(relativeList);
		if (it != relativeSizes.end())
		{
			if (*it->second > startPos)
			{
				*it->second -= count;
			}
		}
	}
};

#endif
