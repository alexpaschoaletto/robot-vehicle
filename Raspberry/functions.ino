void stringifyJSON(request_t *r, char outgoing[]){
  String json;
  JsonDocument msg;
  msg["action"] = r->action;
  if(r->leader) msg["leader"] = r->leader;
  serializeJson(msg, json);
  clear(outgoing);
  copy(json, outgoing);
}

void stringifyJSON(state_t *s, char outgoing[]){
  String json;
  JsonDocument msg;
  msg["iSD"] = s->isSelfDriving;
  msg["ldr"] = s->leader;
  msg["prd"] = s->period;
  msg["str"] = s->steering;
  serializeJson(msg, json);
  clear(outgoing);
  copy(json, outgoing);
}


bool parseJSON(char incoming[], command_t *comm, request_t *req){
  JsonDocument json;
  DeserializationError error = deserializeJson(json, incoming);
  if(error) return false;
  
  clear(comm);
  comm->steering = (long) json["steering"];
  comm->period = (long) json["period"];

  clear(req);
  String act = json["action"];
  copy(act, req->action);
  req->leader = (int) json["leader"];
  return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////

void uartSetup(){
  Uart.setTX(UART_TX);
  Uart.setRX(UART_RX);
  Uart.begin(115200);
  Usb.begin(115200);
}


void statusSetup(){
  unsigned long now = micros();
  status.next = now + status.period;
}


void ioSetup(){
  pinMode(LED, OUTPUT);
  pinMode(EN, OUTPUT);
  pinMode(M0, OUTPUT);
  pinMode(M1, OUTPUT);
  pinMode(M2, OUTPUT);
}


void motorSetup(){
  initializeMotor(&engine);
  initializeMotor(&steering);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void command(command_t *instructions){
  unsigned long now = micros();
  if(instructions->period != 0){
    engine.step.period.next = now + engine.step.period.current;
    if(instructions->period > 0){
      setDirection(&engine, 1);
      setTargetPeriod(&engine, (instructions->period)); 
    } else {
      setDirection(&engine, -1);
      setTargetPeriod(&engine, -(instructions->period));
    }
  } else {
    setTargetPeriod(&engine, 0);
  }
  
  if(instructions->steering != 0){
    steering.step.period.next = now + steering.step.period.current;
    setTargetStep(&steering, (instructions->steering));
  } else {
    setTargetStep(&steering, 0);
  }
}


void beginSelfDrive(){
  unsigned long now = micros();
  state.isSelfDriving = true;
  state.leader = 0;
  trajectory.index = 0;
  trajectory.next = now + trajectory.maneuvers[0].howLong;
  command(&(trajectory.maneuvers[0]));
}


void beginFollow(int leader){
  state.isSelfDriving = true;
  state.leader = leader;
}


void gracefulStop(){
  state.isSelfDriving = false;
  state.leader = 0;
  setTargetPeriod(&engine, 0);
  setTargetStep(&steering, 0);
}


void halt(){
  unsigned long now = micros();
  state.isEmergency = true;
  emergency.index = 0;
  emergency.next = now + emergency.maneuvers[0].howLong;
  command(&(emergency.maneuvers[0]));
}


void uartCycle(){
  if(!uartCheck(Uart, &uartInput)) return;
  uartSend(Usb, (char *)uartInput.buffer);
  
  command_t instructions;
  request_t request;
  if(!parseJSON((char *) uartInput.buffer, &instructions, &request)){
    clear(&uartInput);
    return;
  }

  if(!state.isEmergency && !equals(request.action, "null")){
    if(equals(request.action, "self-drive")){
      beginSelfDrive();
    } else if(equals(request.action, "follow") && request.leader){
      beginFollow(request.leader);
    } else if(equals(request.action, "stop")){
      gracefulStop();
    } else if(equals(request.action, "emergency")){
      halt();
    }
    stringifyJSON(&request, status.message);
    uartSend(status.message);
    clear(status.message);
    clear(&uartInput);
    return;
  }
  
  command(&instructions);
  clear(&uartInput);
}


void usbCycle(){
  if(!uartCheck(Usb, &usbInput)) return;
  uartSend(Uart, (char *)usbInput.buffer);
  clear(&usbInput);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void uartLoop(){
  usbCycle();
  uartCycle();
}


void statusLoop(){
  unsigned long now = micros();
  if(now < status.next) return;
  state.period = (int)(engine.step.direction * engine.step.period.current);
  state.steering = (int)(steering.step.count.current);
  stringifyJSON(&state, status.message);
  uartSend(Uart, status.message);
  status.next += status.period;
}


void motorLoop(){
  unsigned long now = micros();
  run(&engine, now);
  run(&steering, now);
}


void autonomousLoop(){
  if((!state.isEmergency) && (!state.isSelfDriving || state.leader)) return;
  unsigned long now = micros();
  trajectory_t *action = (state.isEmergency)? &emergency : &trajectory; 
  if(now < action->next) return;
  if(action == &emergency){
    if((action->index + 1) == TRAJECTORY_MANEUVERS) {
      state.isEmergency = false;
      gracefulStop();
      return;
    }
  }
  action->index = (action->index + 1) % TRAJECTORY_MANEUVERS;
  action->next = now + action->maneuvers[action->index].howLong;
  command(&(action->maneuvers[action->index]));
}



