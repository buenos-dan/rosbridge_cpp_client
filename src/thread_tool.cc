#include "thread_tool.h"

namespace common {
namespace tools {

const std::string kUnknown = "unknown";

std::map<u_int32_t, string> ThreadTool::name_map_;
std::mutex ThreadTool::mutex_;

void ThreadTool::SetCurrentThreadName(const string &name) {
  u_int32_t tid = GetThreadId();
  mutex_.lock();
  name_map_[tid] = name;
  mutex_.unlock();
}

const string &ThreadTool::GetThreadName(u_int32_t tid) {
  std::unique_lock<std::mutex> lock(mutex_);
  auto it = name_map_.find(tid);
  if (it == name_map_.end()) {
    return kUnknown;
  }
  return it->second;
}

bool ThreadTool::BindCPU(int cpu_id) {
  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);
  CPU_SET(cpu_id, &cpuset);
  int rc = pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
  return rc == 0;
}

bool ThreadTool::BindCPU(int cpu_id, std::thread &thread) {
  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);
  CPU_SET(cpu_id, &cpuset);
  int rc = pthread_setaffinity_np(thread.native_handle(), sizeof(cpu_set_t),
                                  &cpuset);
  return rc == 0;
}

string ThreadTool::ToString(Level level) {
  switch (level) {
  case Level::kHigh:
    return "High";
  case Level::kMiddle:
    return "Middle";
  case Level::kLow:
    return "Low";
  case Level::kNone:
    return "None";
  default:
    break;
  }
  std::stringstream ss;
  ss << "Unknown: " << (int)level;
  return ss.str();
}

} // namespace tools
} // namespace common
