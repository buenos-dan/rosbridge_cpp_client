#pragma once
//#include <libwebsockets.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <string>

#include <ixwebsocket/IXUserAgent.h>
#include <ixwebsocket/IXWebSocket.h>

class RosBridgeWsClient {
public:
  RosBridgeWsClient(std::string remote_addr);

  bool reset();

  void addClient(std::string client_name); //增加客户端

  void advertise(const std::string &client_name, const std::string &topic,
                 const std::string &type,
                 const std::string &id = ""); //注册发送信息

  void publish(const std::string &topic, const rapidjson::Document &msg,
               const std::string &id = ""); //发布数据信息

  int err_count_;
  std::string url_;
  /*
    //订阅已有topic
    void subscribe(const std::string &client_name, const std::string &topic,
                   const InMessage &callback, const std::string &id = "",
                   const std::string &type = "", int throttle_rate = -1,
                   int queue_length = -1, int fragment_size = -1,
                   const std::string &compression = "");
  */
private:
  ix::WebSocket *webSocket_;
};