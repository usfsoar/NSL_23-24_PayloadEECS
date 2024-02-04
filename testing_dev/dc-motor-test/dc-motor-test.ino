#include "DCMotor.h"

DCMotor motor(9, 50, 50);

void setup() {
 motor.DC_SETUP();
}

void loop() {
 motor.DC_MOVE();
}
