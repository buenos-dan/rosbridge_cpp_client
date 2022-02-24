#include "rosbridge_helper.h"
#include <unistd.h>

int main() {

  std::string greet = "hello world";

  for (int i = 0; i < 10000; i++) {
    rosbridge_send_string("/greet/channel_1", greet);
    usleep(10000); // 10ms
  }
}