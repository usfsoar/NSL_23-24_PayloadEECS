#include "MyVL53L0X.h"

void MyVL53L0X::begin() {
  try{
  bool succ = sensor.init();
  Serial.print("initialized sensor");
  delay(100);
  sensor.setTimeout(500);
  // Start continuous back-to-back mode (take readings as fast as possible).
  // To use continuous timed mode instead, provide a desired inter-measurement period in ms (e.g., sensor.startContinuous(100)).
  Serial.println("Distance Sensor Intialized: "+String(succ));
  sensor.startContinuous();
  }
  catch (String error){
    Serial.println("Error: VL53L0X Begin: "+error);
  }
}

uint16_t MyVL53L0X::readDistance() {
  uint16_t Distancemm;
  for (int i=0; i<2; i++){
    Distancemm = sensor.readRangeContinuousMillimeters();
    if (sensor.timeoutOccurred() || Distancemm==65535) {
      Serial.print(" TIMEOUT");
      this->begin();
      Distancemm=65535; // Return 0 if a timeout occurs
    }
    else{
      break;
    }
  }
  return Distancemm;
}

