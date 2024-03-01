#include "MyVL53L0X.h"

void MyVL53L0X::begin() {
  sensor.init();
  sensor.setTimeout(500);
  // Start continuous back-to-back mode (take readings as fast as possible).
  // To use continuous timed mode instead, provide a desired inter-measurement period in ms (e.g., sensor.startContinuous(100)).
  sensor.startContinuous();
}

uint16_t MyVL53L0X::readDistance() {
  uint16_t Distancemm = sensor.readRangeContinuousMillimeters();
  if (sensor.timeoutOccurred()) {
    Serial.print(" TIMEOUT");
    return 0; // Return 0 if a timeout occurs
  }
  return Distancemm;
}

