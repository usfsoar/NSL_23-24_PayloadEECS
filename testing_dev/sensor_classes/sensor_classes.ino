#include "soar_imu.h"
SOAR_IMU imu_sensor;
void setup(){
  Serial.begin(9600);
  imu_sensor.BNO_SETUP();
}
void loop(){

}