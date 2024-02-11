#include "Arduino.h"
#include "DCMotor.h"

void DCMotor::DC_SETUP() {
  Serial.begin(9600); 
  pinMode(pwmPin, OUTPUT); 
}

void DCMotor::DC_MOVE(int speed, uint32_t time) {
  // Map speed to highDelay and lowDelay
  highDelay = map(abs(speed), 0, 100, 50, 500);
  lowDelay = map(abs(speed), 0, 100, 50, 1);
  uint32_t startTime;
  for(startTime = millis(); millis()-startTime < time;){
    for(int i=0; i<2; i++){
      int val = 255;
      int del = highDelay;
      if(iter) val = 0; 
      if(iter) del = lowDelay; 
      analogWrite(pwmPin, 255); 
      //Serial.print(val);
      //Serial.print("\n");
      iter = !iter; 
      delay(del);
    }
  }
  
}

void DCMotor::DC_STOP() {
  analogWrite(pwmPin, 0);
}

