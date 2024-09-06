uint32_t getLength(char buf[]){
  for(int i=0; true; i++){
    if(buf[i] == '\x00') return i;
  }
}


void led(bool state){
  digitalWrite(LED, state);
}


void blink(int period){
  led(!digitalRead(LED));
  delay(period);
  led(!digitalRead(LED));
}


void clear(char what[]){
  memset(what, 0, BUF_SIZE);
}


void clear(char what[], uint32_t length){
  memset(what, 0, length);
}


void clear(bool *what){
  *what = false;
}


void clear(int *what){
  *what = 0;
}

void clear(long *what){
  *what = 0;
}


void clear(request_t *r){
  clear(r->action);
  clear(&(r->leader));
}


void clear(command_t *c){
  clear(&(c->steering));
  clear(&(c->period));
}


void clear(uartInput_t *i){
  clear(i->buffer);
  clear(&(i->hasMessage));
  clear(&(i->charCount));
}


bool equals(char one[], const char two[], int end){
  for(int i = 0; i < end; i++){
    if(one[i] != two[i]) return false;
    if(one[i] == '\0') return (two[i] == '\0');
  }
  return true;
}


bool equals(char one[], const char two[]){
  return equals(one, two, BUF_SIZE);
}

bool equals(String one, const char two[]){
  return equals((char *) one.c_str(), two, BUF_SIZE);
}

bool equals(uartInput_t *one, const char two[], int end){
  return equals(one->buffer, two, end);
}


bool equals(uartInput_t *one, const char two[]){
  int end = (one->charCount);
  if(end == 0) return false;
  return equals(one->buffer, two, end);
}


void copy(char origin[], char destination[]){
  for(int i = 0; true; i++){
    if (origin[i]=='\x00') return;
    destination[i]=origin[i];
  }
}


void copy(const char origin[], char destination[]){
  copy((char *)origin, destination);
}


void copy(String origin, char destination[]){
  copy((char *)origin.c_str(), destination);
}


bool uartCheck(Stream &serial, uartInput_t *input){ 
  while(serial.available()){
    input->buffer[input->charCount] = serial.read();
    if(input->buffer[input->charCount]=='\n' || input->buffer[input->charCount]=='\x00'){ 
      input->buffer[input->charCount]='\x00';                     
      input->hasMessage = true;
      break;
    } else {
      input->charCount++;
      input->hasMessage = false;
    }
  }
  return input->hasMessage;
}


void uartSend(Stream &serial, char message[]){
  serial.print(message);
  serial.print(UART_END_CHARACTER);
}


void uartSend(Stream &serial, const char* message){
  serial.print(message);
  serial.print(UART_END_CHARACTER);
}


void uartSend(Stream &serial, String message){
  serial.print(message);
  serial.print(UART_END_CHARACTER);
}


void uartSend(String message){
  uartSend(Usb, message);
  uartSend(Uart, message);
}


void uartSend(char message[]){
  uartSend(Usb, message);
  uartSend(Uart, message);
}


void uartSend(const char* message){
  uartSend(Usb, message);
  uartSend(Uart, message);
}