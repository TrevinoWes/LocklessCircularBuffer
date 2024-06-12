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

	bool full(const uint32_t& pshIdx, const uint32_t& popIdx) {
		uint32_t size = 0;
		if (popIdx > pshIdx) {
			size = UINT32_MAX - popIdx + pshIdx + 1;
		} else {
			size = pshIdx - popIdx;
		}
		return size == capacity;
	}

	bool empty(const uint32_t& pshIdx, const uint32_t& popIdx) {
		return pshIdx == popIdx;
	}
	
protected:
	// Protected to allow instant overflow testing
    alignas(hardware_destructive_interference_size) std::atomic<uint32_t> pshIdx_{0};
	alignas(hardware_destructive_interference_size) std::atomic<uint32_t> popIdx_{0};
    alignas(hardware_destructive_interference_size) uint32_t cachedPshIdx_{0};
	alignas(hardware_destructive_interference_size) uint32_t cachedPopIdx_{0};

	// Pad to stop shared cache lines for popIdx
	char padding[hardware_destructive_interference_size - sizeof(decltype(popIdx_))];

	static_assert(std::atomic<decltype(pshIdx_)>::is_always_lock_free);
	static_assert(std::atomic<decltype(popIdx_)>::is_always_lock_free);
public:

	CircularSPSCQueue(size_t size) : capacity(size), buffer(capacity, T()),
									 pshIdx_(0), popIdx_(0) {
		if(capacity == 0 || (capacity & (capacity - 1)) != 0) {
			throw std::invalid_argument("capacity must be a power of 2 and greater than 0");
		}
	};

	bool push(const T& val) {
		auto pshIdx = pshIdx_.load(std::memory_order_relaxed);
		if (full(pshIdx, cachedPopIdx_)) {
			cachedPopIdx_ = popIdx_.load(std::memory_order_acquire);
			if (full(pshIdx, cachedPopIdx_)) {
				return false;
			}
		}
		buffer[pshIdx & (capacity - 1)] = std::move(val);
		pshIdx_.store(pshIdx + 1, std::memory_order_release);
		return true;
	};

	bool pop(T& val) {
		auto popIdx = popIdx_.load(std::memory_order_relaxed);
		if(empty(cachedPshIdx_, popIdx)) {
			cachedPshIdx_ = pshIdx_.load(std::memory_order_acquire);
			if (empty(cachedPshIdx_, popIdx)) {
				return false;
			}
		}
		val = std::move(buffer[popIdx & (capacity - 1)]);
		popIdx_.store(popIdx + 1, std::memory_order_release);
		return true;
	};
};

