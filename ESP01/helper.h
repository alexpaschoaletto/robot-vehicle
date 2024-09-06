#ifndef _APP_HELPER_H_
#define _APP_HELPER_H_

#include "constants.h"
#include "types.h"


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


void clear(char what[], uint32_t length){
  memset(what, 0, length);
}


void clear(char what[]){
  clear(what, BUF_SIZE);
}


void clear(bool *what){
  *what = false;
}


void clear(int *what){
  *what = 0;
}


void clear(uartInput_t *i){
  clear(i->buffer);
  clear(&(i->hasMessage));
  clear(&(i->charCount));
}


void append(char toBeAppended[], char target[]){
  int i = 0, j = 0;
  for(; target[i]; i++);
  for(; toBeAppended[j]; j++){
    target[i] = toBeAppended[j];
    i++;
  }
}

void append(int toBeAppended, char target[]){
  String stringified(toBeAppended);
  append((char *)stringified.c_str(), target);
}


void copyUntil(char origin[], char destination[], char breakpoint, int totalBreakpoints){
  int i = 0;
  clear(destination);
  for(int b = 0; b < totalBreakpoints; b++){
    for(;;){
      destination[i] = origin[i];
      if(origin[i] == breakpoint) break;
      i++;
    }
    i++;
  }
}


void copy(char origin[], char destination[]){
  clear(destination);
  for(int i = 0; true; i++){
    if (origin[i]=='\x00') return;
    destination[i]=origin[i];
  }
}

void copy(const char origin[], char destination[]){
  copy((char *) origin, destination);
}

void copy(String origin, char destination[]){
  copy(origin.c_str(), destination);
}


bool equals(char one[], const char two[], int end){
  for(int i = 0; i < end; i++){
    if(one[i] == '\x00') return (i > 0);
    if(one[i] != two[i]) return false;
  }
  return true;
}


bool equals(uartInput_t *one, const char two[], int end){
  if(end == 0) return false;
  return equals(one->buffer, two, end);
}


bool equals(char one[], const char two[]){
  return equals(one, two, BUF_SIZE);
}


bool equals(String one, const char two[]){
  char *msg = (char *) one.c_str();
  return equals(msg, two, one.length());
}


bool equals(uartInput_t *one, const char two[]){
  return equals(one->buffer, two, one->charCount);
}


bool isHttpGetRequest(char request[]){
  char possibleRequest[BUF_SIZE];
  clear(possibleRequest);
  for(int i=0; i < BUF_SIZE; i++){
    possibleRequest[i] = request[i];
    if(equals(possibleRequest, "GET /")){
      return true;
    }
  }
  return false;
}


void filterHttpGetRequest(String *request){
  if(!isHttpGetRequest((char *)(request->c_str()))) return;
  int offset = 5;                                             // offset is 5 because we want to skip the "GET /" part
  const char *httpRequest = request->c_str();
  char command[BUF_SIZE];
  clear(command);

  for(int i = 0; i < (BUF_SIZE - offset); i++){
    if(httpRequest[i + offset] == ' '){                       // and white space is the end of the command
      request->clear();
      request->concat(command);
      return;
    }
    command[i] = httpRequest[i + offset];
  }
}


void notify(String m){
  if (m == NULL) return;
  Serial.print(m);
  Serial.print(UART_END_CHARACTER);
}


#endif