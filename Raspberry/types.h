#include "constants.h"

#ifndef types_h
#define types_h

#include <FreeRTOS.h>
#include <task.h> 

struct status_t {
  char message[BUF_SIZE];
  unsigned long period;
  unsigned long next;
};

struct command_t {
  long period;
  long steering;
  long howLong;
};

struct target_t {
  long period;
  long step;
};

struct trajectory_t {
  command_t maneuvers[TRAJECTORY_MANEUVERS];
  int index;
  unsigned long next;
};

struct request_t {
  char action[BUF_SIZE];
  int leader;
};

struct state_t {
  bool isSelfDriving;
  bool isEmergency;
  int leader;
  int period;
  int steering;
  char action[BUF_SIZE];
};

struct period_t {
  unsigned long current;         // período atual
  unsigned long next;            // próximo período
  unsigned long min;             // período mínimo (vel máx) - usado para aceleração 
  unsigned long max;             // período máximp (vel mín) - usado para aceleração
  unsigned long delta;           // razão com que o o período oscila entre máximo e mínimo
};

struct count_t {
  long current;
  long min;
  long max;
};

struct step_t {
  int direction;
  period_t period;
  count_t count;
};

struct unipolar_t {
  int pins[4];
  int sequence[UNIPOLAR_SEQUENCE_LENGTH][4];
  int sequenceLength;
  int index;
  step_t step;
  target_t target;
};

struct stepper_t {
  int pins[2];
  step_t step;
  target_t target;
};

struct uartInput_t {
  char buffer[BUF_SIZE];
  bool hasMessage;
  int charCount;   
};

struct ultrasonicSensor_t {
  int trigPin;
  int echoPin;
  int distance;
  int r;
  int readings[SAMPLES];
  unsigned long period;
  unsigned long next;
};

#endif
