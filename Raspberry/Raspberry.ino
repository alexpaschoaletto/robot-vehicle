//use earle philhower III's Raspberry Pi Pico board support for this
#include <ArduinoJson.h>
#include "pegasoboard.h"
#include "constants.h"
#include "types.h"
#include "ultrasonic.h"

uartInput_t usbInput = {"", false, 0};                                          
uartInput_t uartInput = {"", false, 0};


unipolar_t steering = {
  .pins = {SDA, SCL, RST, SLP},
  .sequence = {
    {1, 0, 0, 0},
    {1, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 1},
    {0, 0, 0, 1},
    {1, 0, 0, 1}
  },
  .sequenceLength = UNIPOLAR_SEQUENCE_LENGTH,
  .index = 0,
  .step = {
    .direction = 0,
    .period = {850, 0, 850, 850, 0}, 
    .count = {0, -350, 350}
  },
  .target = {
    .period = 0,
    .step = 0
  }
};


stepper_t engine = {
  .pins = {STEP_BASE, DIR_BASE},
  .step = {
    .direction = 0,
    .period = {1300, 0, 600, 1300, 1},
    .count = {0, 0, 0}
  },
  .target = {
    .period = 0,
    .step = 0
  }
};


ultrasonicSensor_t ultrasonic = {
  .trigPin = SCK,
  .echoPin = MISO,
  .distance = 0,
  .r = 0,
  .readings = {},
  .period = 200000,
  .next = 0
};

state_t state = {
  .isSelfDriving = false,
  .isEmergency = false,
  .leader = 0,
  .period = 0,
  .steering = 0
};

trajectory_t trajectory = {
  .maneuvers = AUTONOMOUS_PATH, 
  .index = 0, .next = 0
};

trajectory_t emergency = {
  .maneuvers = EMERGENCY_PATH,
  .index = 0, .next = 0
};

status_t status = {"", 50000, 0};

void setup() {
  ioSetup();
  uartSetup();
  statusSetup();
}

void loop(){
  uartLoop();
  statusLoop();
  autonomousLoop();
  //sensorLoop();
}

void setup1() {
  initializeMotor(&engine);
  initializeMotor(&steering);
  delay(5000);
  setMotors(ENABLED);
}

void loop1(){
  motorLoop();
}


