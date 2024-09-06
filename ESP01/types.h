#include "constants.h"

#ifndef types_h
#define types_h


struct wifi_t {
  char ssid[BUF_SIZE];
  char password[BUF_SIZE];
  char ip[BUF_SIZE];
  bool connected;
  bool dhcp;
};

struct uartInput_t {
  char buffer[BUF_SIZE];
  bool hasMessage;
  int charCount;
};

#endif
