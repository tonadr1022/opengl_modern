#include "e_assert.h"

void engine::assert::HandleAssert(const char *msg, const char *condition, const char *filename,
                                  uint64_t lineNumber) {
  spdlog::critical("Assert failed: {}\nCondition: {}\nFile: {}\nLine: {}\n", msg, condition,
                   filename, lineNumber);
}
