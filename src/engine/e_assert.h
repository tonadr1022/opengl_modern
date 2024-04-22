#pragma once

#include <spdlog/logger.h>

#include <cstdint>

namespace assert {
void HandleAssert(const char* msg, const char* condition, const char* filename,
                  uint64_t lineNumber);
}

#ifndef NDEBUG
#define EASSERT_MSG(stmt, msg)                              \
  do {                                                      \
    if (!(stmt)) {                                          \
      assert::HandleAssert(msg, #stmt, __FILE__, __LINE__); \
      std::abort();                                         \
    }                                                       \
  } while (0)

#define EASSERT(stmt)                                                      \
  do {                                                                     \
    if (!(stmt)) {                                                         \
      assert::HandleAssert("Assertion Failed", #stmt, __FILE__, __LINE__); \
      std::abort();                                                        \
    }                                                                      \
  } while (0)

#else

#define EASSERT_MSG(stmt, msg) \
  do {                         \
  } while (0)

#define EASSERT(stmt) \
  do {                \
  } while (0)

#endif  // !NDEBUG
