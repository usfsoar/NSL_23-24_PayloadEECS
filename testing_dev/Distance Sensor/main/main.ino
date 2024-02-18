#include <Wire.h>
#include "MyVL53L0X.h"

MyVL53L0X mySensor;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  mySensor.begin();
}

void loop() {
  uint16_t dist=mySensor.readDistance();
  //if(dist<8000){
    Serial.print("Distance,");
    Serial.println(dist);
    delay(100);
  //}
  
}

