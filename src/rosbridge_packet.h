#ifndef INTERFACE_ROSBRIDGE_PACK
#define INTERFACE_ROSBRIDGE_PACK

#include <google/protobuf/util/json_util.h>

#include <memory>

#include "ros_msg.pb.h"

using namespace std;

bool packet_ros_msg(std::shared_ptr<RosPacket> pack, string ros_topic,
                    google::protobuf::Message *msg, string *frame = NULL) {
  pack->set_topic(ros_topic);
  if (frame != NULL) {
    pack->set_frame(*frame);
  }
  if (string(msg->GetDescriptor()->name()) == string("String")) {
    pack->set_msg_type("std_msgs/String");
  } else {
    std::cout << __FILE__
              << " unsupport type : " << msg->GetDescriptor()->name()
              << std::endl;
    return false;
  }

  // parse name from msg type
  std::string json_string;
  google::protobuf::util::JsonPrintOptions JP;
  JP.preserve_proto_field_names = true;
  MessageToJsonString(*msg, &json_string, JP);

  pack->set_msg(json_string);
  return true;
}

void SimpleSet(std::shared_ptr<RosPacket> pack, string topic, string frameID) {
  return;
}

bool get_ros_msg_name() { return true; }

#endif
