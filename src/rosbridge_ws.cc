#include "rosbridge_ws.hpp"
#include <iostream>
#include <memory>
#include <unistd.h>

RosBridgeWsClient::RosBridgeWsClient(std::string remote_addr) {

  if (strncmp(remote_addr.c_str(), "ws://", 5) == 0) {
    url_ = remote_addr;
  } else {
    url_ = "ws://" + remote_addr;
  }
  reset();
  return;
}

bool RosBridgeWsClient::reset() {
  webSocket_ = new ix::WebSocket();

  webSocket_->setUrl(url_);

  webSocket_->setOnMessageCallback([this](const ix::WebSocketMessagePtr &msg) {
    if (msg->type == ix::WebSocketMessageType::Message) {
      std::cout << "received message: " << msg->str << std::endl;
      std::cout << " " << std::flush;
    } else if (msg->type == ix::WebSocketMessageType::Open) {
      std::cout << "rosbridge established: " << this->url_ << std::endl;
      std::cout << " " << std::flush;
      this->err_count_ = 0;
    } else if (msg->type == ix::WebSocketMessageType::Error) {
      // Maybe SSL is not configured properly
      this->err_count_++;
      std::cout << "rosbridge error: " << msg->errorInfo.reason << std::endl;
      std::cout << " " << std::flush;
    }
  });

  webSocket_->start();

  sleep(1);
  // sleep(1);
  return true;
}

void RosBridgeWsClient::addClient(std::string client_name) { return; }

void RosBridgeWsClient::advertise(const std::string &client_name,
                                  const std::string &topic,
                                  const std::string &type,
                                  const std::string &id) {
  std::string message = "\"op\":\"advertise\", \"topic\":\"" + topic +
                        "\", \"type\":\"" + type + "\"";

  if (id.compare("") != 0) {
    message += ", \"id\":\"" + id + "\"";
  }
  message = "{" + message + "}";

  std::cout << message << std::endl;

  webSocket_->send(message);
  return;
}

void RosBridgeWsClient::publish(const std::string &topic,
                                const rapidjson::Document &msg,
                                const std::string &id) {
  rapidjson::StringBuffer strbuf;
  rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
  msg.Accept(writer);

  std::string message = "\"op\":\"publish\", \"topic\":\"" + topic +
                        "\", \"msg\":" + strbuf.GetString();

  if (id.compare("") != 0) {
    message += ", \"id\":\"" + id + "\"";
  }
  message = "{" + message + "}";

  // std::cout << message << std::endl;

  auto ret = webSocket_->send(message);

  // if (ret.success == false)
  // std::cout << ret.success << " " << ret.payloadSize << " " << ret.wireSize
  //        << std::endl;

  return;
}

#if 0
//订阅已有topic
void subscribe(const std::string &client_name, const std::string &topic,
               const InMessage &callback, const std::string &id = "",
               const std::string &type = "", int throttle_rate = -1,
               int queue_length = -1, int fragment_size = -1,
               const std::string &compression = "");
#endif
