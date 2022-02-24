#include "rosbridge_man.hpp"

DEFINE_string(rosbridge_addr, "127.0.0.1:9090",
              "rosbridge addrs and port ,default is 127.0.0.1:9090");

RosBridgeMan::RosBridgeMan() {
  rbc_ = std::make_shared<RosBridgeWsClient>(FLAGS_rosbridge_addr);
  rbc_->addClient("ros_bridge_topic_advertiser");

  std::function<void(const std::shared_ptr<RosPacket> &)> thread_fun =
      std::bind(&RosBridgeMan::loop, this, std::placeholders::_1);
  send_thread_.reset(
      new common::tools::LooperThread<std::shared_ptr<RosPacket>>(
          thread_fun, "slam_display_thread", 10));
};

bool RosBridgeMan::send_msg(const std::shared_ptr<RosPacket> &msg0) {
  send_thread_->Push(msg0);
  return true;
};

void RosBridgeMan::loop(const std::shared_ptr<RosPacket> &msg0) {
  mutex.lock();

  static int error_flag = 0;
  // when we can't connect
  if (rbc_->err_count_ > 1) {
    topic_map_.clear();
    error_flag++;
    // rbc_->err_count_ = 10; // count for 10s
    // return;
  } else {
    std::map<std::string, std::string>::iterator iter;
    iter = topic_map_.find(msg0->topic());

    if (iter == topic_map_.end()) {
      static int iii = 0;
      time_t timep;
      time(&timep);
      topic_map_[msg0->topic()] = "topic_" + std::to_string(timep + iii++);
      rbc_->addClient(topic_map_[msg0->topic()]);
      rbc_->advertise(topic_map_[msg0->topic()], msg0->topic(),
                      msg0->msg_type());
      // cout<<"Find, the value is"<<iter->second<<endl;
    }
    rapidjson::Document d;
    d.Parse(msg0->msg().c_str());
    rbc_->publish(msg0->topic(), d);
  }
  mutex.unlock();
  /*
  std::cout << topic_map_[msg0->topic()] << ":" << msg0->topic() << " "
            << msg0->msg_type() << " " << msg0->msg() << std::endl;
  */
  return;
};
