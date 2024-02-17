#include "soar_imu.h"
SOAR_IMU imu_sensor;
void setup(){
  Serial.begin(9600);
  imu_sensor.BNO_SETUP();
}
void loop(){
  float* accel = imu_sensor.GET_ACCELERATION();
  for(int i=0; i<3; i++){
    Serial.print("Accel array: ");
    Serial.print(accel[i]);
    Serial.print(" ");
  }
  Serial.print("\n");
  delay(2000);
}
