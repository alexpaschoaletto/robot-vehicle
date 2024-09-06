//use earle philhower III's Raspberry Pi Pico board support for this

#include <ArduinoJson.h>
#include "pegasoboard.h"
#include "constants.h"
#include "types.h"


uartInput_t usbInput = {
  .buffer = "",
  .hasMessage = false,
  .charCount = 0
};


uartInput_t uartInput = {
  .buffer = "",
  .hasMessage = false,
  .charCount = 0
};


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


nema_t engine = {
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


status_t status = {
  .message = "",
  .period = 50000,
  .next = 0
};

// Raspberry Pi Pico has two cores; below, core 0 setup and loop
void setup() {
  ioSetup();
  uartSetup();
  statusSetup();
}

void loop(){
  uartLoop();                   //checks incoming commands from either serial or USB
  statusLoop();                 //reports the robot status
  autonomousLoop();             //handles the autonomous path if enabled
}


// and below, core 1 setup and loop (yes, it's easy as that)
void setup1() {
  initializeMotor(&engine);
  initializeMotor(&steering);
  delay(5000);
  setMotors(ENABLED);
}

void loop1(){
  motorLoop();                  //runs the motors if they are required to move
}


