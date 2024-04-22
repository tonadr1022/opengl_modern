#include "e_assert.h"

#include <spdlog/logger.h>
#include <spdlog/spdlog.h>

void assert::HandleAssert(const char *msg, const char *condition, const char *filename,
                          uint64_t lineNumber) {
  spdlog::critical("Assert failed: {}\nCondition: {}\nFile: {}\nLine: {}\n", msg, condition,
                   filename, lineNumber);
}
