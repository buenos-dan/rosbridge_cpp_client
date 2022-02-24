#ifndef COMMON_TOOLS_THREAD_TOOL_H_
#define COMMON_TOOLS_THREAD_TOOL_H_

#include <map>
#include <mutex>
#include <pthread.h>
#include <string>
#include <thread>

#include "thread_utils.h"

namespace common {
namespace tools {

using common::utils::GetThreadId;
using std::string;

class ThreadTool {

public:
  enum class Level { kHigh = -3, kMiddle = -2, kLow = -1, kNone = 0 };

  static inline bool SetLevel(Level level) {
    return SetThreadPriority((int)level);
  }

  static inline Level GetLevel() { return (Level)GetThreadPriority(); }

  static inline std::string GetLevelStr() {
    return ToString((Level)GetThreadPriority());
  }

  static void SetCurrentThreadName(const string &name);

  static inline const string &GetCurrentThreadName() {
    u_int32_t tid = GetThreadId();
    return GetThreadName(tid);
  }

  static const string &GetThreadName(u_int32_t tid);

  /**
   * @brief binds current thread to the specified CPU.
   * @param cpu_id the id of the specified cpu to bind.
   * @return whether the binding to CPU operation successful.
   */
  static bool BindCPU(int cpu_id);

  /**
   * @brief binds the given thread to the specified CPU.
   * @param cpu_id the id of the specified cpu to bind.
   * @param thread the thread to be bound to specified cpu.
   * @return whether the binding to CPU operation successful.
   */
  static bool BindCPU(int cpu_id, std::thread &thread);

private:
  inline static bool SetThreadPriority(int niceval) {
    return setpriority(0, GetThreadId(), niceval) == 0;
  }

  inline static int GetThreadPriority() {
    return getpriority(0, GetThreadId());
  }

  static string ToString(Level level);

  static std::mutex mutex_;
  static std::map<uint32_t, string> name_map_;
};

} // namespace tools
} // namespace common

#endif
