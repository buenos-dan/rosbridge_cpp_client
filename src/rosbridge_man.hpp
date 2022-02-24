#pragma once

#include <mutex>

#include "looper_thread.h"
#include "rapidjson/writer.h"
#include "ros_msg.pb.h"
#include "rosbridge_ws.hpp"
#include <memory.h>

DECLARE_string(rosbridge_addr);
// DEFINE_string(rosbridge_addr, "127.0.0.1:9090",
//               "rosbridge addrs and port ,default is 127.0.0.1:9090");

class RosBridgeMan {
public:
  inline static RosBridgeMan *instance() {
    static RosBridgeMan rosman;
    return &rosman;
  };

  RosBridgeMan();

  virtual ~RosBridgeMan() {
    if (send_thread_ != nullptr) {
      send_thread_->Cancel(true);
    }
  }

  bool send_msg(const std::shared_ptr<RosPacket> &msg0);

  void loop(const std::shared_ptr<RosPacket> &msg0);

private:
  std::map<std::string, std::string> topic_map_;
  std::shared_ptr<RosBridgeWsClient> rbc_;
  std::mutex mutex;
  std::shared_ptr<common::tools::LooperThread<std::shared_ptr<RosPacket>>>
      send_thread_;
};