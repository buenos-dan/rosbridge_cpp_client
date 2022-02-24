#include "rosbridge_helper.h"

#include "rosbridge_man.hpp"
#include "rosbridge_packet.h"

bool rosbridge_send_string(const std::string &channel, const string &msg) {

  String string_msg;

  string *data = string_msg.mutable_data();
  *data = msg;

  auto rosmsg = std::make_shared<RosPacket>();
  if (packet_ros_msg(rosmsg, channel, &string_msg)) {
    RosBridgeMan::instance()->send_msg(rosmsg);
    return true;
  }

  return false;
}
