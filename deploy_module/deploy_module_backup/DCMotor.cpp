#include "Arduino.h"
#include "DCMotor.h"

void DCMotor::DC_SETUP() {
  // Serial.begin(9600); 
  // pinMode(pwmPin, OUTPUT); 
  esc.attach(pwmPin); // Attach the ESC signal pin to the servo object
  for(int i=0; i<3; i++){
    esc.writeMicroseconds(0); // Stop
    delay(500); // Stop for 2 second

  }
  
}

void DCMotor::DC_MOVE(int speed) {
  if (started) return;

  int freq;
  if (speed > 0){
    //freq = 2000;
    freq = map(abs(speed), 1, 100, 1800, 1100);
  } else if (speed < 0) {
    //freq = 1099;
    freq = map(abs(speed), -1, -100, 1099, 2000);
  } else {
    freq = 0;
  }
  
  for(int i=0; i<3; i++){
    Serial.println("Moving motor");
    esc.writeMicroseconds(freq); // 100% speed
    delay(500); // Move forward for 2 seconds
  }
  started = true;
  
  // Map speed to highDelay and lowDelay
  // highDelay = map(abs(speed), 0, 100, 50, 500);
  // lowDelay = map(abs(speed), 0, 100, 50, 1);
  // uint32_t startTime;
  // for(startTime = millis(); millis()-startTime < time;){
  //   for(int i=0; i<2; i++){
  //     int val = 255;
  //     int del = highDelay;
  //     if(iter) val = 0; 
  //     if(iter) del = lowDelay; 
  //     analogWrite(pwmPin, 255); 
  //     //Serial.print(val);
  //     //Serial.print("\n");
  //     iter = !iter; 
  //     delay(del);
  //   }
  // }
  
}

void DCMotor::DC_STOP() {
   for(int i=0; i<3; i++){
    esc.writeMicroseconds(0); // Stop
    delay(500); // Stop for 2 second

  }
  //analogWrite(pwmPin, 0);
  started=false;
}

