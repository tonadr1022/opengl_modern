#pragma once

#include "../pch.h"

namespace util {

class Timer {
 public:
  void Start();
  double GetElapsedSeconds();
  double GetElapsedMS();
  void Reset();

 private:
  std::chrono::time_point<std::chrono::high_resolution_clock> start_time_;
  std::chrono::time_point<std::chrono::high_resolution_clock> end_time_;
};

}  // namespace util
