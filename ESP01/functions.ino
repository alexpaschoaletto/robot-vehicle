bool uartCheck(){
  if(Serial.available()){
    uartInput.buffer[uartInput.charCount] = Serial.read();
    if(uartInput.buffer[uartInput.charCount]=='\n' || uartInput.buffer[uartInput.charCount]=='\x00'){ 
      uartInput.buffer[uartInput.charCount]='\x00';                     
      uartInput.hasMessage = true;
    } else {
      uartInput.charCount++;
      uartInput.hasMessage = false;
    }
  }
  return uartInput.hasMessage;
}


void prompt(String m){
  if(m == NULL) return;
  blink(50);
  notify(m);
  for(;;){
    if(uartCheck()) return; 
  }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void requestWifi(){
  prompt(MSG_REQUEST_WIFI);
  copy(uartInput.buffer, wifi.ssid);
  clear(&uartInput);
  
  while(!uartCheck());
  copy(uartInput.buffer, wifi.password);
  clear(&uartInput);

  while(!uartCheck());
  copy(uartInput.buffer, wifi.ip);
  clear(&uartInput);
}


void updateIP(){
  String ipString = WiFi.localIP().toString();
  copy((char *)ipString.c_str(), wifi.ip);
}

void setStaticIpFromHost(char host[]){
  String oldIP = WiFi.localIP().toString();
  char newIP[BUF_SIZE/2];
  
  copyUntil((char *) oldIP.c_str(), newIP, '.', 3);                          // copia até o 3° caractere '.' (ou seja, todo o IP original menos o host)
  append(host, newIP);
  
  IPAddress ip;                                      
  ip.fromString(newIP);                          
  WiFi.config(ip, WiFi.gatewayIP(), WiFi.subnetMask());
}


void requestData(){
  if(!SKIP_WIFI_REQUEST){
    clear(wifi.ssid);
    clear(wifi.password);
    clear(wifi.ip);
    requestWifi();
  }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void gpioSetup(){
  clear(&uartInput);
  pinMode(LED, OUTPUT);
  delay(3000);
}


void uartSetup(){
  Serial.begin(115200);
}


void httpSetup(){
  server.begin();
}


void socketSetup(){
  websocketSetup();
}

void wifiSetup(bool isReconnecting){
  if(isReconnecting) WiFi.disconnect();
  WiFi.begin(wifi.ssid, wifi.password);
  for(;;) {
    blink(50);
    delay(1000);
    if(WiFi.status() == WL_CONNECTED) break;
  }
  if(equals(wifi.ip, WIFI_DHCP)){
    wifi.dhcp = true;
    updateIP();
  } else {
    setStaticIpFromHost((char *) ID);
    updateIP();
  }
  String connectionMsg = MSG_CONNECTION_OK + String(" ") + String(wifi.ip);
  notify(connectionMsg);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void uartLoop(){
  if(!uartCheck()) return;
  websocketBroadcast(uartInput.buffer);
  clear(&uartInput);
}


void wifiLoop(){
  if(WiFi.status() == WL_DISCONNECTED){
    notify("disconnected");
    if(wifi.dhcp){
      clear(wifi.ip);
      copy(WIFI_DHCP, wifi.ip);
    }
    wifiSetup(true);
    server.begin();
  }
}


void httpLoop(){
  WiFiClient client = server.available();
  if(client){                                           
    String request = "";
    for(;;){
      if(!client.available()) break;
      request+=client.readStringUntil('\n');
    }
    processHTTP(client, request);
    client.stop();
  }
}


void socketLoop(){
  websocketLoop();
}




