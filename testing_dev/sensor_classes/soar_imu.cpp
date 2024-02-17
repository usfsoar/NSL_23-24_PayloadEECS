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
  Serial.print("Successful\n");
}

float *SOAR_IMU::GET_ACCELERATION(void) {

//read imu linear acceleration vector and return an array of float
//upon failure, call setup class from this function reset the BNO055

float *acceleration = new float[3];

  if(fail_count <= 20){
    imu::Vector<3> a = bno.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);

    acceleration[0] = a.x();
    acceleration[1] = a.y();
    acceleration[0] = a.z();

    int zeroes_count = 0;
    for(int i=0; i<3; i++){
      if (acceleration[i] == 0.0){
        fail_count ++;
      }
      else {
        fail_count = 0;
      }
    }
  }
  else {
    Serial.print("fail_count > 20, resetting...");
    this->BNO_SETUP();
  }  
  return acceleration;

}
