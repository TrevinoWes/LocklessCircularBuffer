#pragma once

#include <vector>
#include <atomic>

template <class T>
class CircularSPSCQueue {
private:
	size_t capacity;
	std::vector<T> buffer;

	bool full() {
		uint32_t size = 0;
		if (popIdx > pshIdx) {
			size = UINT32_MAX - popIdx + pshIdx + 1;
		}
		else {
			size = pshIdx - popIdx;
		}
		return size == capacity;
	}
protected:
	// Protected to allow instant overflow testing
	std::atomic<uint32_t> pshIdx;
	std::atomic<uint32_t> popIdx;

public:

	CircularSPSCQueue(size_t size) : capacity(size), buffer(capacity, T()),
									 pshIdx(0), popIdx(0) {};

	bool push(T& val) {
		if (full()) {
			return false;
		}
		buffer[pshIdx % capacity] = val;
		pshIdx.fetch_add(1);
		return true;
	};

	bool pop(T& val) {
		if (pshIdx == popIdx) {
			return false;
		}

		val = buffer[popIdx % capacity];
		popIdx.fetch_add(1);
		return true;
	};
};
