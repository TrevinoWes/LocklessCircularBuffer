#include <string>
#include <thread>
#include <chrono>
#include "gtest/gtest.h"
#include "CircularSPSCQueue.h"

namespace {
	int TEST_CASE_COUNT = 2000;

	TEST(CBTest, Int) {
		CircularSPSCQueue<int> buffer(1);
		int retVal = 0;
		for (int i = 0; i < TEST_CASE_COUNT; ++i) {
			EXPECT_TRUE(buffer.push(i));
			EXPECT_TRUE(buffer.pop(retVal));
			EXPECT_EQ(i, retVal);
		}
	}

	TEST(CBTest, String) {
		CircularSPSCQueue<std::string> buffer(1);
		std::string retVal, strI;
		for (int i = 0; i < TEST_CASE_COUNT; ++i) {
			strI = std::to_string(i);
			EXPECT_TRUE(buffer.push(strI));
			EXPECT_TRUE(buffer.pop(retVal));
			EXPECT_EQ(strI, retVal);
		}
	}

	TEST(CBTest, PushFull) {
		int size = 5;
		CircularSPSCQueue<int> buffer(size);
		for(int i = 0; i < size; ++i) {
			EXPECT_TRUE(buffer.push(i));
		}

		EXPECT_FALSE(buffer.push(size));

		int retVal;
		buffer.pop(retVal);
		EXPECT_EQ(0, retVal);

	}

	TEST(CBTest, PopEmpty) {
		int size = 5;
		CircularSPSCQueue<int> buffer(size);
		int retVal;
		EXPECT_FALSE(buffer.pop(retVal));

		EXPECT_TRUE(buffer.push(size));
		EXPECT_TRUE(buffer.pop(retVal));
		EXPECT_EQ(size, retVal);
	}

	template <class t>
	class ExtendedCircularSPSCQueue: public CircularSPSCQueue<t> {
	public:
		ExtendedCircularSPSCQueue(size_t size): CircularSPSCQueue<t>(size) {};
		void setPushIndex(uint32_t pshI) {CircularSPSCQueue<t>::pshIdx_ = pshI;}	
		void setPopIndex(uint32_t popI) {CircularSPSCQueue<t>::popIdx_ = popI;}
	};


	TEST(CBTest, Wrapping) {
		ExtendedCircularSPSCQueue<uint32_t> buffer(2);
		uint32_t retVal;
		// Force overflow testing
		buffer.setPushIndex(UINT32_MAX - 1);
		buffer.setPopIndex(UINT32_MAX - 1);
  
		uint32_t first = 0, second = 1;
		EXPECT_TRUE(buffer.push(first));
		EXPECT_TRUE(buffer.push(second));
		EXPECT_FALSE(buffer.push(second));
		EXPECT_TRUE(buffer.pop(retVal));
		EXPECT_EQ(first, retVal);
		EXPECT_TRUE(buffer.pop(retVal));
		EXPECT_EQ(second, retVal);
		EXPECT_FALSE(buffer.pop(retVal));
	}

	TEST(CBTest, MulitThreaded) {
		CircularSPSCQueue<uint32_t> buffer(50);
		uint32_t start = 5, test_count = TEST_CASE_COUNT, val;
		
		std::thread pushThread([&buffer, &test_count, &start](){
			for(uint32_t i = start; i < test_count; ++i) {
				if(!buffer.push(i)) {
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
					EXPECT_TRUE(buffer.push(i));
				}
			}
		});
		
		std::thread popThread([&buffer, &test_count, &start, &val](){
			for(uint32_t i = start; i < test_count; ++i) {
				if(!buffer.pop(val)) {
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
					EXPECT_TRUE(buffer.pop(val));
				}
				EXPECT_EQ(val, i);
			}
		});

		pushThread.join();
		popThread.join();
		EXPECT_FALSE(buffer.pop(val));
	}
}