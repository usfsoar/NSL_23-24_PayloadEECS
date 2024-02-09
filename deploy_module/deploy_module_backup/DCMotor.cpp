#include "Arduino.h"
#include "DCMotor.h"

void DCMotor::DC_SETUP() {
  Serial.begin(9600); 
  pinMode(pwmPin, OUTPUT); 
}

void DCMotor::DC_MOVE(int speed, uint32_t timer) {
  unsigned long startTime = millis(); // Capture start time
  unsigned long endTime = startTime + timer; // Calculate end time
      
  // Map speed to highDelay and lowDelay
  highDelay = map(abs(speed), 0, 100, 50, 500);
  lowDelay = map(abs(speed), 0, 100, 50, 1);
  
  // Loop until the specified time has elapsed
  while (millis() < endTime) {
    int val = 255;
    int del = highDelay;
    if (iter) val = 0;
    if (iter) del = lowDelay;
    analogWrite(pwmPin, val);
    iter = !iter;
    delay(del); // This delay controls the speed of the motor
  }

  // Optional: stop the motor after the loop
  analogWrite(pwmPin, 0); // Stop the motor by setting PWM to 0

}

void DCMotor::DC_STOP() {
  analogWrite(pwmPin, 0);
}

