#ifndef _APP_WEBSOCKET_H_
#define _APP_WEBSOCKET_H_

#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include "constants.h"
#include "types.h"
#include "helper.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct socket_t {
  WebSocketsServer server;
  int connectedDevices;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

socket_t socket = {
  .server = WebSocketsServer(DEFAULT_SOCKET_PORT),
  .connectedDevices = 0
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void websocketBroadcast(char message[]){
  if(!socket.connectedDevices) return;
  socket.server.broadcastTXT(message);
}


static void onMessage(char *payload) {
  Serial.println(payload);
}


static void serverEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
  switch(type) {
    case WStype_CONNECTED: 
      socket.connectedDevices++;  
    break;
    case WStype_DISCONNECTED:
      socket.connectedDevices--;
    break;
    case WStype_TEXT:
      onMessage((char *) payload);
    break;
  }
}


void websocketSetup(){
  socket.server.onEvent(serverEvent);
  socket.server.begin();
}


void websocketLoop(){
  socket.server.loop();
}


#endif
