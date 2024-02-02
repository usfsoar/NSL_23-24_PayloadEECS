#include "SOAR_IMU.h"

SOAR_IMU::SOAR_IMU() {
  this->bno = Adafruit_BNO055(55);
  // Constructor implementation
  // Initialize variables if needed

}
// Implement other methods here

void SOAR_IMU::BNO_SETUP() {

  Serial.println("Orientation Sensor Test"); Serial.println("");
  
  /* Initialise the sensor */
  if(!this->bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  
  delay(1000);
    
  this->bno.setExtCrystalUse(true);
  Serial.print("Successful");
}


