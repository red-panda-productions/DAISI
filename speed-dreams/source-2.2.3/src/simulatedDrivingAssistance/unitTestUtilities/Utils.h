#pragma once
#include <future>
#include <csetjmp> // is needed for ASSERT_DURATION_LE
#include <csignal>

/// A big header file that contains functions that can be used in unit tests



/// <summary>
/// Executes a statement, and fails if it takes longer than the secs parameter
/// </summary>
/// <param name="secs"> The timeout parameter </param>
/// <param name="stmt"> The statement to be executed </param>
#define ASSERT_DURATION_LE(secs, stmt) { \
  std::promise<bool> completed; \
  auto stmt_future = completed.get_future(); \
  std::thread([&](std::promise<bool>& completed) { \
    stmt; \
    completed.set_value(true); \
  }, std::ref(completed)).detach(); \
  if(stmt_future.wait_for(std::chrono::seconds(secs)) == std::future_status::timeout) \
    GTEST_FATAL_FAILURE_("       timed out (> " #secs \
    " seconds). Check code for infinite loops"); \
}

/// <summary>
/// Tests if 2 messages are equal to each other
/// </summary>
/// <param name="msg1"> The first message </param>
/// <param name="msg2"> The second message </param>
/// <param name="size"> The size of the 2 messages </param>
/// <returns> Whether the test failed or succeeded </return>
inline bool TestMessageEqual(const char* msg1, const char* msg2, int size);


/// <summary>
/// Generates a random string for testing
/// </summary>
/// <param name="dataBuffer"> The buffer in which the string will be written </param>
/// <param name="stringLength"> The length of the string (must be strictly smaller than the length of the buffer) </param>
inline void GenerateRandomCharArray(char* dataBuffer, int stringLength);

/// @brief returns a random float between 0 and max
/// @param max
/// @return
inline float randomFloat(const float max = 1)
{
    return (float)rand()/(float)(RAND_MAX/1) * max;
}

/// @brief compares two values. If their difference is lower than the tolerance, return true.
#define ASSERT_ALMOST_EQ(f1, f2, tolerance) ASSERT_LE(abs(f1-f2), tolerance)