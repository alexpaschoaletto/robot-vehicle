#include "constants.h"               
#include "types.h"
#include "websocket.h"
#include "webpage.h"

uartInput_t uartInput = {"", false, 0};                     // uart input {messageReceived, message is complete, character count}

wifi_t wifi = {                                             // wifi settings {ssid, password, ip address, is connected, is dhcp}
  DEFAULT_WIFI_SSID,
  DEFAULT_WIFI_PASSWORD,
  DEFAULT_IP_ADDRESS,
  false, false
};

void setup() {
  gpioSetup();
  led(LOW);
  uartSetup();
  requestData();
  wifiSetup(false);
  httpSetup();
  socketSetup();
  led(HIGH); 
}

void loop() {
  uartLoop();
  wifiLoop();
  httpLoop();
  socketLoop();
}