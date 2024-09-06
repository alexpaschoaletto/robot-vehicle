#include "pegasoboard.h"

#ifndef constants_h
#define constants_h

#define BUF_SIZE 100

#define LED SDA
#define RES 2
#define SAMPLES 10
#define BYJ_STEPS_PER_REVOLUTION 
#define NEMA_STEPS_PER_REVOLUTION 200
#define DEFAULT_STEP_PERIOD 150
#define DEFAULT_INITIAL_STEP_COUNT 0

#define UNIPOLAR_SEQUENCE_LENGTH 8
#define NEMA_MIN_DELAY 45

#define TRAJECTORY_MANEUVERS 12
#define NOTHING { 0, 0, 0 }

#define STRAIGHT_PATH {                                     \
  { .period = 600, .steering = 0, .howLong = 6000000 },     \
  { .period = -600, .steering = 0, .howLong = 6000000 }     \
}

#define CURVY_PATH {                                        \
  { .period = 600, .steering = 0, .howLong = 3000000 },     \
  { .period = 0, .steering = 0, .howLong = 300000 },        \
  { .period = 0, .steering = 350, .howLong = 300000 },      \
  { .period = 800, .steering = 350, .howLong = 8000000 },   \
  { .period = 0, .steering = 350, .howLong = 300000 },      \
  { .period = 0, .steering = 0, .howLong = 300000 },        \
  { .period = 600, .steering = 0, .howLong = 3000000 },     \
  { .period = 0, .steering = 0, .howLong = 300000 },        \
  { .period = 0, .steering = 350, .howLong = 300000 },      \
  { .period = 800, .steering = 350, .howLong = 8000000 },   \
  { .period = 0, .steering = 350, .howLong = 300000 },      \
  { .period = 0, .steering = 0, .howLong = 300000 }         \
}

#define EMERGENCY_PATH {                                    \
  { .period = 1000, .steering = -350, .howLong = 1500000 }, \
  { .period = 1000, .steering = 0, .howLong = 1000000 },    \
  { .period = 1000, .steering = 350, .howLong = 1500000 },  \
  { .period = 1000, .steering = 0, .howLong = 1000000 },    \
  NOTHING,                                                  \
  NOTHING,                                                  \
  NOTHING,                                                  \
  NOTHING,                                                  \
  NOTHING,                                                  \
  NOTHING,                                                  \
  NOTHING,                                                  \
  NOTHING                                                   \
}

#define AUTONOMOUS_PATH STRAIGHT_PATH

#define ENABLED LOW
#define DISABLED HIGH

#define STEP 0
#define DIR 1

#define Usb Serial
#define Uart Serial1
#define UART_END_CHARACTER "\n"

#endif