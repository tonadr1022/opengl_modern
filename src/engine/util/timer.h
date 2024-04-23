#pragma once

#include <chrono>

class Timer {
 public:
  void Start() { start_time_ = std::chrono::high_resolution_clock::now(); };
  double GetElapsedSeconds() {
    std::chrono::duration<double> duration =
        std::chrono::high_resolution_clock::now() - start_time_;
    return duration.count();
  }

  double GetElapsedMS() {
    std::chrono::duration<double, std::milli> duration =
        std::chrono::high_resolution_clock::now() - start_time_;
    return duration.count();
  }

  void Reset() { start_time_ = std::chrono::high_resolution_clock::now(); }

 private:
  std::chrono::time_point<std::chrono::high_resolution_clock> start_time_;
  std::chrono::time_point<std::chrono::high_resolution_clock> end_time_;
};
