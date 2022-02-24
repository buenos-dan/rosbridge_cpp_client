#ifndef COMMON_UTILS_THREAD_UTIL_H_
#define COMMON_UTILS_THREAD_UTIL_H_

#include <glog/logging.h>
#include <string>
#include <sys/resource.h>
#include <sys/syscall.h>
#include <unistd.h>

namespace common {
namespace utils {

inline uint32_t GetThreadId() { return syscall(SYS_gettid); }

inline uint32_t GetPid() { return syscall(SYS_getpid); }

class ThreadPriority {

public:
  enum class Level { kHigh = -20, kMiddle = -10, kLow = 0 };

  static bool SetLevel(Level level) { return SetThreadPriority((int)level); }

  static Level GetLevel() { return (Level)GetThreadPriority(); }

  static std::string GetLevelStr() {
    return ToString((Level)GetThreadPriority());
  }

  static std::string ToString(Level level) {
    switch (level) {
    case Level::kHigh:
      return "High";
    case Level::kMiddle:
      return "Middle";
    case Level::kLow:
      return "Low";
    default:
      break;
    }
    std::stringstream ss;
    ss << "Unknown: " << (int)level;
    return ss.str();
  }

private:
  inline static bool SetThreadPriority(int niceval) {
    int result = setpriority(0, GetThreadId(), niceval);
    if (result != 0) {
      LOG(WARNING) << "SetThreadPriority Failed: " << result << " "
                   << strerror(errno);
      return false;
    }
    return true;
  }

  inline static int GetThreadPriority() {
    return getpriority(0, GetThreadId());
  }
};

} // namespace utils
} // namespace common

#endif
