#pragma once
#include <future>
#include <csetjmp> // is needed for ASSERT_DURATION_LE
#include <csignal>

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

bool TestMessageEqual(const char* p_msg1, const char* p_msg2, int p_size);

void GenerateRandomCharArray(char* p_dataBuffer, int p_stringLength);


