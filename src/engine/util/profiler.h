#pragma once

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <string>
#include <thread>

#include "engine/resource/paths.h"

// #define PROFILING 0
#ifdef PROFILING
#define PROFILE_SCOPE(name) ProfileTimer timer##__line__(name)
#define PROFILE_FUNCTION() PROFILE_SCOPE(__FUNCTION__)
#else
#define PROFILE_SCOPE(name)
#define PROFILE_FUNCTION()
#endif

struct ProfileResult {
  const std::string name = "Default";
  uint64_t start{0};
  uint64_t end{0};
  size_t thread_id{0};
};

class Profiler {
 public:
  static Profiler& Get() {
    static Profiler instance;
    return instance;
  }
  ~Profiler() { output_file_.end(); }
  void WriteProfile(const ProfileResult& r) {
    std::lock_guard<std::mutex> lock(lock_);
    if (profile_count_++ > 0) output_stream_ << ",";
    std::string name = r.name;
    std::replace(name.begin(), name.end(), '"', '\'');
    output_stream_ << R"({)";
    output_stream_ << R"("cat": "function",)";
    output_stream_ << R"("dur":)" << (r.end - r.start) << ",";
    output_stream_ << R"("name": ")" << name << "\",";
    output_stream_ << R"("ph": "X",)";
    output_stream_ << R"("pid": 0,)";
    output_stream_ << R"("tid": )" << r.thread_id << ",";
    output_stream_ << R"("ts":)" << r.start;
    output_stream_ << "}";
  }

 private:
  std::ofstream output_stream_;
  std::string output_file_{GET_PATH("result.json")};
  size_t profile_count_;
  std::mutex lock_;

  Profiler() {
    output_stream_ = std::ofstream(output_file_);
    WriteHeader();
  }
  void WriteHeader() { output_stream_ << R"({"otherData": {},"traceEvents":[)"; }
  void WriteFooter() { output_stream_ << R"(]})"; }
};

class ProfileTimer {
 public:
  ProfileTimer() = default;
  explicit ProfileTimer(const std::string& name) : result_({name, 0, 0, 0}) { Start(); }
  ~ProfileTimer() { Stop(); }

  void Start() {
    auto start_time = std::chrono::high_resolution_clock::now();
    result_.start = std::chrono::time_point_cast<std::chrono::microseconds>(start_time)
                        .time_since_epoch()
                        .count();
  };

  void Stop() {
    if (stopped_) return;

    auto end_time = std::chrono::high_resolution_clock::now();

    result_.end = std::chrono::time_point_cast<std::chrono::microseconds>(end_time)
                      .time_since_epoch()
                      .count();
    result_.thread_id = std::hash<std::thread::id>{}(std::this_thread::get_id());
    Profiler::Get().WriteProfile(result_);

    stopped_ = true;
  }

 private:
  bool stopped_{false};
  ProfileResult result_;
};
