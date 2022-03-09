#pragma once
#include <future>
#include <csetjmp> // is needed for ASSERT_DURATION_LE
#include <csignal>
#include <gtest/gtest.h>
/// A big header file that contains functions that can be used in unit tests

/// @brief      Executes a statement, and fails if it takes longer than the secs parameter
/// @param secs The timeout parameter
/// @param stmt The statement to be executed
#define ASSERT_DURATION_LE(p_secs, p_stmt) { \
  std::promise<bool> completed; \
  auto stmt_future = completed.get_future(); \
  std::thread([&](std::promise<bool>& completed) { \
    p_stmt; \
    completed.set_value(true); \
  }, std::ref(completed)).detach(); \
  if(stmt_future.wait_for(std::chrono::seconds(secs)) == std::future_status::timeout) \
    GTEST_FATAL_FAILURE_("       timed out (> " #p_secs \
    " seconds). Check code for infinite loops"); \
}

void TestStringEqual(const char* p_msg1, const char* p_msg2, int p_size);

void GenerateRandomCharArray(char* p_dataBuffer, int p_stringLength);

// parameterized test cases (E.G. TEST_CASE(suite,exampleTest,exampleFunc,(1,"hi"))
#define TEST_CASE(test_suite_name,test_name,methodName,params)\
	TEST(test_suite_name,test_name)\
	{ \
		methodName params;\
	}

// combinatorial testing

#define TEST_COMBINATORIAL2(test_suite_name,methodName,arr1,count1,arr2,count2) \
	TEST(test_suite_name,methodName ## Combinatorial ## count1 ## v ## count2)\
	{\
	    for(int a = 0; a < count1; a++)\
		{\
			for(int b = 0; b < count2; b++) \
			{\
				methodName(arr1[a],arr2[b]);\
			}\
		} \
	}

#define TEST_COMBINATORIAL3(test_suite_name,methodName,arr1,count1,arr2,count2,arr3,count3) \
	TEST(test_suite_name,methodName ## Combinatorial ## count1 ## v ## count2 ## v ## count3)\
	{\
	    for(int a = 0; a < count1; a++)\
		{\
			for(int b = 0; b < count2; b++) \
			{\
				for(int c = 0; c < count3; c++) \
				{\
					methodName(arr1[a],arr2[b],arr3[c]);\
				}\
			}\
		} \
	}

#define TEST_COMBINATORIAL4(test_suite_name,methodName,arr1,count1,arr2,count2,arr3,count3,arr4,count4) \
	TEST(test_suite_name,methodName ## Combinatorial ## count1 ## v ## count2 ## v ## count3 ## v ## count4)\
	{\
	    for(int a = 0; a < count1; a++)\
		{\
			for(int b = 0; b < count2; b++) \
			{\
				for(int c = 0; c < count3; c++) \
				{\
					for(int d = 0; d < count4; d++) \
					{\
						methodName(arr1[a],arr2[b],arr3[c],arr4[d]);\
					}\
				}\
			}\
		} \
	}

