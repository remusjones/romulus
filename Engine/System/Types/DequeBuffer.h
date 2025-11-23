#pragma once
#include "EASTL/deque.h"

template<typename T>
class DequeBuffer
{
public:
	explicit DequeBuffer(const size_t size)
		: buffer(size), size(size)
	{
	}

	void AddElement(T value)
	{
		buffer.emplace_back(value);
		if (buffer.size() > size)
		{
			buffer.pop_front();
		}
	}

	eastl::deque<T> buffer;
	size_t size;
};
