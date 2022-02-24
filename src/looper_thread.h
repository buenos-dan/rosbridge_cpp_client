#pragma once

#include <glog/logging.h>

#include <atomic>
#include <condition_variable>
#include <list>
#include <memory>
#include <mutex>
#include <thread>

#include "thread_tool.h"
#include <functional>

namespace common {
namespace tools {

using std::mutex;
using std::thread;

struct Void {};

template <class Message> class LooperThread {
public:
  typedef std::shared_ptr<Message> MessagePtr;
  typedef std::function<void(const MessagePtr &)> PtrHandler;
  typedef std::function<void(const Message &)> DataHandler;

  /**
   *  @param Handler 处理函数(返回指针形式)
   *  @param max_count 队列中消息最大数量 (-1: 无限制)
   */
  LooperThread(const PtrHandler &handler, const std::string &name,
               size_t max_count = -1)
      : handler_(handler), name_(name), max_count_(max_count), cancel_(false),
        forced_cancel_(false), size_(0) {
    thread_.reset(new thread(&LooperThread::ThreadLoop, this));
  }

  /**
   *  @param Handler 处理函数(返回数据本身形式)
   *  @param max_count 队列中消息最大数量 (-1: 无限制)
   */
  LooperThread(const DataHandler &handler, const std::string &name,
               size_t max_count = -1)
      : name_(name), max_count_(max_count), cancel_(false),
        forced_cancel_(false), size_(0) {
    handler_ = [handler](const MessagePtr &msg) { handler(*msg); };
    thread_.reset(new thread(&LooperThread::ThreadLoop, this));
  }

  virtual ~LooperThread() {
    if (thread_ != nullptr && thread_->joinable()) {
      LOG(ERROR) << "LooperThread does not be canceled, name: " << name_;
      Cancel(true);
    }
  }

  void set_max_count(size_t max_count) {
    mutex_.lock();
    max_count_ = max_count;
    mutex_.unlock();
  }

  /**
   *  推送一个Msg的共享指针进入队列，等待处理
   *  @return 如果有数据被挤掉，返回该数据
   */
  inline size_t Push(const MessagePtr &t_ptr, MessagePtr *deserted = nullptr) {
    std::unique_lock<mutex> lock(mutex_);
    if (cancel_) {
      return size_;
    }
    if (size_ == max_count_) {
      CHECK(size_ <= max_count_) << size_ << " " << max_count_;
      if (deserted != nullptr) {
        *deserted = list_.front();
      }
      list_.pop_front();
      size_--;
    }
    list_.push_back(t_ptr);
    size_++;
    condition_.notify_one();
    return size_;
  }

  /**
   *  推送一个Msg进入队列，等待处理
   *  会产生一次数据Copy
   */
  inline size_t Push(const Message &t, MessagePtr *deserted = nullptr) {
    return Push(std::make_shared<Message>(t), deserted);
  }

  /**
   *  推送一个Msg进入队列，等待处理
   *  右值方案，避免不必要的copy
   */
  inline size_t Push(Message &&t, MessagePtr *deserted = nullptr) {
    return Push(std::make_shared<Message>(t), deserted);
  }

  /**
   *  关闭
   *  @param safely
   *  true: 强制关闭
   *      即使有数据在等待处理也会强行停止。但会等待正在处理中的任务处理完毕
   *  false: 安全关闭
   *      会等待所有任务执行完毕后再退出
   */
  inline void Cancel(bool forced = true) {
    std::unique_lock<mutex> lock(mutex_);
    if (forced_cancel_) {
      return;
    }
    forced_cancel_ = forced;
    if (cancel_) {
      return;
    }
    cancel_ = true;
    condition_.notify_one();
    lock.unlock();
    if (thread_->joinable()) {
      thread_->join();
    }
  }

  /**
   *  仅用于Debug使用
   */
  inline size_t size() { return size_; }

  inline std::unique_ptr<thread> &ThreadPtr() { return thread_; }

private:
  void ThreadLoop() {
    ThreadTool::SetCurrentThreadName(name_);
    while (true) {
      std::unique_lock<mutex> lock(mutex_);
      if (!cancelled() && list_.empty()) {
        condition_.wait(lock);
      }
      if (cancelled()) {
        break;
      }
      CHECK(!list_.empty()) << "LooperThread Error : list empty when pop";
      MessagePtr message = list_.front();
      list_.pop_front();
      size_--;
      lock.unlock();
      handler_(message);
    }
  }

  inline bool cancelled() {
    return forced_cancel_ || (list_.empty() && cancel_);
  }

  PtrHandler handler_;
  std::string name_;
  std::list<MessagePtr> list_;
  mutex mutex_;
  std::condition_variable_any condition_;
  std::unique_ptr<thread> thread_;
  size_t max_count_;
  bool cancel_;
  bool forced_cancel_;
  std::atomic<size_t> size_;
};

template <> class LooperThread<void> : public LooperThread<Void> {
public:
  typedef std::function<void()> Handler;

  LooperThread<void>(const Handler &handler, const std::string &name,
                     size_t max_count = -1)
      : LooperThread<Void>(
            [handler](const std::shared_ptr<const Void> &) { handler(); }, name,
            max_count) {}

  inline void Notify() { LooperThread<Void>::Push(Void()); }
};

} // namespace tools
} // namespace common
