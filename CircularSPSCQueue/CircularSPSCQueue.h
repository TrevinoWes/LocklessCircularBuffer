#pragma once

#include <vector>
#include <atomic>
#include <new>

#ifdef __cpp_lib_hardware_interference_size
    using std::hardware_constructive_interference_size;
    using std::hardware_destructive_interference_size;
#else
    // 64 bytes on x86-64 │ L1_CACHE_BYTES │ L1_CACHE_SHIFT │ __cacheline_aligned │ ...
    constexpr static std::size_t hardware_constructive_interference_size = 64;
    constexpr static std::size_t hardware_destructive_interference_size = 64;
#endif

template <class T>
class CircularSPSCQueue {
private:
	size_t capacity;
	std::vector<T> buffer;

	bool full(const std::atomic<uint64_t>& pshIdx, const std::atomic<uint64_t>& popIdx) {
		uint32_t size = 0;
		if (popIdx > pshIdx) {
			size = UINT32_MAX - popIdx + pshIdx + 1;
		} else {
			size = pshIdx - popIdx;
		}
		return size == capacity;
	}

	bool empty(const std::atomic<uint64_t>& pshIdx, const std::atomic<uint64_t>& popIdx) {
		return (pshIdx == popIdx);
	}

protected:
	// Protected to allow instant overflow testing
    alignas(hardware_destructive_interference_size) std::atomic<uint32_t> pshIdx_;
	alignas(hardware_destructive_interference_size) std::atomic<uint32_t> popIdx_;

	// Pad to stop shared cache lines for popIdx
	char padding[hardware_destructive_interference_size - sizeof(decltype(popIdx_))];

	static_assert(std::atomic<decltype(pshIdx_)>::is_always_lock_free);
	static_assert(std::atomic<decltype(popIdx_)>::is_always_lock_free);
public:

	CircularSPSCQueue(size_t size) : capacity(size), buffer(capacity, T()),
									 pshIdx_(0), popIdx_(0) {};

	bool push(const T& val) {
		auto pshIdx = pshIdx_.load(std::memory_order_relaxed);
		auto popIdx = popIdx_.load(std::memory_order_acquire);
		if (full(pshIdx, popIdx)) {
			return false;
		}
		buffer[pshIdx % capacity] = val;
		pshIdx_.store(pshIdx + 1, std::memory_order_release);
		return true;
	};

	bool pop(T& val) {
		auto popIdx = popIdx_.load(std::memory_order_relaxed);
		auto pshIdx = pshIdx_.load(std::memory_order_acquire);
		if (empty(pshIdx, popIdx)) {
			return false;
		}

		val = buffer[popIdx % capacity];
		popIdx_.store(popIdx + 1, std::memory_order_release);
		return true;
	};
};

