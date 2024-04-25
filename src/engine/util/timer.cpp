#include "timer.h"

namespace util {

void Timer::Start() { start_time_ = std::chrono::high_resolution_clock::now(); };
double Timer::GetElapsedSeconds() {
  std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - start_time_;
  return duration.count();
}

double Timer::GetElapsedMS() {
  std::chrono::duration<double, std::milli> duration =
      std::chrono::high_resolution_clock::now() - start_time_;
  return duration.count();
}

void Timer::Reset() { start_time_ = std::chrono::high_resolution_clock::now(); }

}  // namespace util
