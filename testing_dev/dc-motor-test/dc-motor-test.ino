#include "DCMotor.h"

DCMotor motor(9, 50, 50);

void setup() {
  motor.DC_SETUP();
}

void loop() {
  int speed = 50; // replace with desired speed between -100 and 100
  motor.DC_MOVE(speed);
}

//moveStepper
