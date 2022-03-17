#pragma once
#include <future>
#include <csetjmp> // is needed for ASSERT_DURATION_LE
#include <csignal>
#include <gtest/gtest.h>
#include "PairWiseTestGenerator.h"
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

/// @brief compares two values. If their difference is lower than the tolerance, return true.
#define ASSERT_ALMOST_EQ(f1, f2, tolerance) ASSERT_LE(abs(f1-f2), tolerance)

// test cases, how to use:
// TEST_CASE(TestSuite,ExampleTest,ExampleFunc,(1,"hi"));

/// @brief					Creates a parameterized test case
/// @param  test_suite_name The name of the test suite
/// @param  test_name		The name of this test
/// @param  methodName		The name of the method that needs to be called
/// @param  params			The parameters
#define TEST_CASE(test_suite_name,test_name,methodName,params)\
	TEST(test_suite_name,test_name)\
	{ \
		methodName params;\
	}

// combinatorial testing, how to use:
// BEGIN_TEST_COMBINATORIAL(suite,name)				   // start the test
// int ages[3] {42,69,22};							   // Define arrays or vectors
// const char* names[3] {"Sem","Debra","Elliot"};
// END_TEST_COMBINATORIAL(PrintAgeName,ages,3,names,3) // end the test with the given arrays

/// @brief					Begin a combinatorial test
/// @param  test_suite_name The name of the test suite
/// @param  test_name		The name of the combinatorial test
#define BEGIN_TEST_COMBINATORIAL(test_suite_name,test_name) \
	TEST(test_suite_name,test_name) \
	{

/// @brief				End a combinatorial test with 2 arrays
/// @param  method_name The method that needs to be called in the test
/// @param  arrname1	The array name of the first array
/// @param  count1		The amount of elements in the first array
/// @param  arrname2	The array name of the second array
/// @param  count2		The amount of elements in the second array
#define END_TEST_COMBINATORIAL2(method_name,arrname1,count1,arrname2,count2) \
		for (int a = 0; a < (count1); a++)\
		{\
			for (int b = 0; b < (count2); b++)\
			{\
				method_name((arrname1)[a], (arrname2)[b]);\
			}\
		}\
	}

/// @brief				End a combinatorial test with 2 arrays
/// @param  method_name The method that needs to be called in the test
/// @param  arrname1	The array name of the first array
/// @param  count1		The amount of elements in the first array
/// @param  arrname2	The array name of the second array
/// @param  count2		The amount of elements in the second array
/// @param  arrname3	The array name of the third array
/// @param  count3		The amount of elements in the third array
#define END_TEST_COMBINATORIAL3(method_name,arrname1,count1,arrname2,count2,arrname3,count3) \
		for (int a = 0; a < (count1); a++)\
		{\
			for (int b = 0; b < (count2); b++)\
			{\
				for(int c = 0; c < (count3); c++)\
				{\
					method_name((arrname1)[a], (arrname2)[b],(arrname3)[c]);\
				}\
			}\
		}\
	}

/// @brief				End a combinatorial test with 2 arrays
/// @param  method_name The method that needs to be called in the test
/// @param  arrname1	The array name of the first array
/// @param  count1		The amount of elements in the first array
/// @param  arrname2	The array name of the second array
/// @param  count2		The amount of elements in the second array
/// @param  arrname3	The array name of the third array
/// @param  count3		The amount of elements in the third array
/// @param  arrname4	The array name of the fourth array
/// @param  count4		The amount of elements in the fourth array
#define END_TEST_COMBINATORIAL4(method_name,arrname1,count1,arrname2,count2,arrname3,count3,arrname4,count4) \
		for (int a = 0; a < (count1); a++)\
		{\
			for (int b = 0; b < (count2); b++)\
			{\
				for(int c = 0; c < (count3); c++)\
				{\
					for(int d = 0; d < (count4); d++)\
					{\
						method_name((arrname1)[a], (arrname2)[b], (arrname3)[c], (arrname4)[d]);\
					}\
				}\
			}\
		}\
	}


template<typename T1, typename T2>
inline void PairWiseTest(void(*func)(T1, T2), T1* input1, int count1, T2* input2, int count2)
{
	PairWiseTestGenerator<2> generator;
	int dimensions[2]{ count1, count2 };

	std::vector<TestCaseInfo>& testCases = *generator.GetTestCases(dimensions);

	for(int i = 0; i < testCases.size(); i++)
	{
		int* Features = testCases[i].Features;
		func(input1[Features[0]], input2[Features[1]]);
	}
}

template<typename T1, typename T2, typename T3>
inline void PairWiseTest(void(*func)(T1, T2, T3), T1* input1, int count1, T2* input2, int count2, T3* input3, int count3)
{
	PairWiseTestGenerator<3> generator;
	int dimensions[3]{ count1, count2, count3};

	std::vector<TestCaseInfo>& testCases = *generator.GetTestCases(dimensions);

	for (int i = 0; i < testCases.size(); i++)
	{
		int* Features = testCases[i].Features;
		func(input1[Features[0]], input2[Features[1]], input3[Features[2]]);
	}
}

template<typename T1, typename T2, typename T3, typename T4>
inline void PairWiseTest(void(*func)(T1, T2, T3, T4), T1* input1, int count1, T2* input2, int count2, T3* input3, int count3, T4* input4, int count4)
{
	PairWiseTestGenerator<4> generator;
	int dimensions[4]{ count1, count2, count3, count4 };

	std::vector<TestCaseInfo>& testCases = *generator.GetTestCases(dimensions);

	for (int i = 0; i < testCases.size(); i++)
	{
		int* Features = testCases[i].Features;
		func(input1[Features[0]], input2[Features[1]], input3[Features[2]], input4[Features[3]]);
	}
}

template<typename T1, typename T2, typename T3, typename T4, typename T5>
inline void PairWiseTest(void(*func)(T1, T2, T3, T4, T5), T1* input1, int count1, T2* input2, int count2, T3* input3, int count3, T4* input4, int count4, T5* input5, int count5)
{
	PairWiseTestGenerator<5> generator;
	int dimensions[5]{ count1, count2, count3, count4, count5 };

	std::vector<TestCaseInfo>& testCases = *generator.GetTestCases(dimensions);

	for (int i = 0; i < testCases.size(); i++)
	{
		int* Features = testCases[i].Features;
		func(input1[Features[0]], input2[Features[1]], input3[Features[2]], input4[Features[3]], input5[Features[4]]);
	}
}

template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
inline void PairWiseTest(void(*func)(T1, T2, T3, T4, T5, T6), T1* input1, int count1, T2* input2, int count2, T3* input3, int count3, T4* input4, int count4, T5* input5, int count5, T6* input6, int count6)
{
	PairWiseTestGenerator<6> generator;
	int dimensions[6]{ count1, count2, count3, count4, count5, count6 };

	std::vector<TestCaseInfo>& testCases = *generator.GetTestCases(dimensions);

	for (int i = 0; i < testCases.size(); i++)
	{
		int* Features = testCases[i].Features;
		func(input1[Features[0]], input2[Features[1]], input3[Features[2]], input4[Features[3]], input5[Features[4]], input6[Features[5]]);
	}
}

template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
inline void PairWiseTest(void(*func)(T1, T2, T3, T4, T5, T6, T7), T1* input1, int count1, T2* input2, int count2, T3* input3, int count3, T4* input4, int count4, T5* input5, int count5, T6* input6, int count6, T7* input7, int count7)
{
	PairWiseTestGenerator<7> generator;
	int dimensions[7]{ count1, count2, count3, count4, count5, count6, count7 };

	std::vector<TestCaseInfo>& testCases = *generator.GetTestCases(dimensions);

	for (int i = 0; i < testCases.size(); i++)
	{
		int* Features = testCases[i].Features;
		func(input1[Features[0]], input2[Features[1]], input3[Features[2]], input4[Features[3]], input5[Features[4]], input6[Features[5]], input7[Features[6]]);
	}
}

template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
inline void PairWiseTest(void(*func)(T1, T2, T3, T4, T5, T6, T7, T8), T1* input1, int count1, T2* input2, int count2, T3* input3, int count3, T4* input4, int count4, T5* input5, int count5, T6* input6, int count6, T7* input7, int count7, T8* input8, int count8)
{
	PairWiseTestGenerator<8> generator;
	int dimensions[8]{ count1, count2, count3, count4, count5, count6, count7, count8 };

	std::vector<TestCaseInfo>& testCases = *generator.GetTestCases(dimensions);

	for (int i = 0; i < testCases.size(); i++)
	{
		int* Features = testCases[i].Features;
		func(input1[Features[0]], input2[Features[1]], input3[Features[2]], input4[Features[3]], input5[Features[4]], input6[Features[5]], input7[Features[6]], input8[Features[7]]);
	}
}