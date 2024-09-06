#ifndef constants_h
#define constants_h

/*
  the robot will connect on address http://your.ip.adress.[ID]
  so for example if your local network is 192.168.0.X, you will
  be able to find this robot on http://192.168.0.252 . Make sure
  to select a static ID that is not in use already by one of the
  existing network devices.
*/
#define ID "252"

/*
  If you skip the wifi request, the ESP will use the ones provided as default.
  the IP will be dynamically calculated from the ID given above, so the macro
  DEFAULT_WIFI_IP below is just a placeholder.
*/
#define SKIP_WIFI_REQUEST true

#define DEFAULT_WIFI_SSID         "your_ssid"
#define DEFAULT_WIFI_PASSWORD     "your_password"
#define DEFAULT_WIFI_IP           "nothing"
#define DEFAULT_SOCKET_PORT 81

#define BUF_SIZE 100
#define LED 2

#define MSG_REQUEST_WIFI "send the wifi"
#define MSG_CONNECTION_OK "connected"
#define UART_END_CHARACTER "\n"

#endif