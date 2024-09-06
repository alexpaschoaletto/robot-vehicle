void cooldown(unipolar_t *motor){
  digitalWrite(motor->pins[0], LOW);
  digitalWrite(motor->pins[1], LOW);
  digitalWrite(motor->pins[2], LOW);
  digitalWrite(motor->pins[3], LOW);
}


void setMotors(bool state){
  digitalWrite(EN, state);
}


void setResolution(int resolution){
  bool m0 = false;
  bool m1 = false;
  bool m2 = false;
  switch(resolution){
    case 1: break;
    case 2:
      m0 = true;
      break;
    case 4:
      m1 = true;
      break;
    case 8:
      m0 = true;
      m1 = true;
      break;
    case 16:
      m2 = true;
      break;
    default:
      m0 = true;
      m2 = true;
    break;
  }
  digitalWrite(M0, m0);
  digitalWrite(M1, m1);
  digitalWrite(M2, m2);
}


void setDirection(nema_t *motor, int direction){
  setMotors((direction != 0)? ENABLED : DISABLED);
  motor->step.direction = direction;
  digitalWrite(motor->pins[DIR], (direction > 0)? 1 : 0); 
}


void setDirection(unipolar_t *motor, int direction){
  motor->step.direction = direction; 
  if(!direction) cooldown(motor);
}


void setTargetPeriod(nema_t *motor, long period){
  if(!period){
    motor->target.period = 0;
    return;
  } else if((motor->step.period.max) && (((unsigned long) period) > motor->step.period.max)){
    motor->target.period = motor->step.period.max;
    return;
  } else if((motor->step.period.min) && (((unsigned long) period) < motor->step.period.min)){
    motor->target.period = motor->step.period.min;
    return;
  } 
  motor->target.period = (unsigned long) period;
}


void setTargetStep(unipolar_t *motor, long target){
  if(motor->step.count.current == target) return;
  setDirection(motor, (motor->step.count.current < target)? 1 : -1);
  if((motor->step.count.max) && (target > motor->step.count.max)){
    motor->target.step = motor->step.count.max;
    return;
  } else if((motor->step.count.min) && (target < motor->step.count.min)){
    motor->target.step = motor->step.count.min;
    return;
  } 
  motor->target.step = target;
}


void initializeMotor(nema_t *motor){
  pinMode(motor->pins[STEP], OUTPUT);
  pinMode(motor->pins[DIR], OUTPUT);
  setResolution(RES);
  setMotors(DISABLED);
}


void initializeMotor(unipolar_t *motor){
  pinMode(motor->pins[0], OUTPUT);
  pinMode(motor->pins[1], OUTPUT);
  pinMode(motor->pins[2], OUTPUT);
  pinMode(motor->pins[3], OUTPUT);
}


bool accelerate(nema_t *motor){  
  if(!motor->target.period){
    motor->step.period.current += motor->step.period.delta;
    return (motor->step.period.current < motor->step.period.max);
  }
  if(motor->step.period.current > motor->target.period){
    motor->step.period.current -= motor->step.period.delta;
  } else if(motor->step.period.current < motor->target.period){
    motor->step.period.current += motor->step.period.delta;
  }
  return true;
}


void run(nema_t  *motor, unsigned long now){
  if(now < motor->step.period.next) return;
  motor->step.period.next += motor->step.period.current;
  if(!motor->step.direction) return;

  digitalWrite(motor->pins[STEP], HIGH);
  delayMicroseconds(NEMA_MIN_DELAY);
  digitalWrite(motor->pins[STEP], LOW);
  motor->step.count.current += motor->step.direction;

  bool isMoving = accelerate(motor);
  if(!isMoving) setDirection(motor, 0);
}


void run(unipolar_t *motor, unsigned long now){
  if(now < motor->step.period.next) return;
  motor->step.period.next += motor->step.period.current;
  if(!motor->step.direction) return;
  
  digitalWrite(motor->pins[0], motor->sequence[motor->index][0]);
  digitalWrite(motor->pins[1], motor->sequence[motor->index][1]);
  digitalWrite(motor->pins[2], motor->sequence[motor->index][2]);
  digitalWrite(motor->pins[3], motor->sequence[motor->index][3]);
  motor->step.count.current += motor->step.direction;

  int nextIndex = motor->index + motor->step.direction;
  switch(nextIndex){
    case -1: motor->index = (UNIPOLAR_SEQUENCE_LENGTH - 1); break;
    case UNIPOLAR_SEQUENCE_LENGTH: motor->index = 0; break;
    default: motor->index = nextIndex;
  }

  bool isMoving = (motor->step.count.current != motor->target.step);
  if(!isMoving) setDirection(motor, 0);
}



