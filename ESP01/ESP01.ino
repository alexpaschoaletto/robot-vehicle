#include "constants.h"               
#include "types.h"
#include "websocket.h"
#include "webpage.h"

uartInput_t uartInput = {
  "",
  .hasMessage = false,
  .charCount = 0
};

wifi_t wifi = {
  DEFAULT_WIFI_SSID,
  DEFAULT_WIFI_PASSWORD,
  DEFAULT_WIFI_IP,
  .connected = false, 
  .dhcp = false
};

void setup() {
  gpioSetup();
  led(LOW);
  uartSetup();
  requestWifiData();
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