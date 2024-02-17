#include <Wire.h>
#include "MyVL53L0X.h"

MyVL53L0X mySensor;

void setup() {
  Serial.begin(9600);
  Wire.begin(D2, D3);

  mySensor.begin();
}

void loop() {
  Serial.print("Distance: ");
  Serial.print(mySensor.readDistance());
  Serial.println(" mm");
  delay(100);
}

