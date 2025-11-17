#pragma once
#include <queue>

template<typename T>
class DequeBuffer
{
public:
	explicit DequeBuffer(const size_t size)
		: size(size)
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

	std::deque<T> buffer;
	size_t size;
};
