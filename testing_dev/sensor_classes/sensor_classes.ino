<<<<<<< Updated upstream
void setup(){

}
void loop(){

=======
// #include "soar_imu.h"
#include "soar_barometer.h"

// SOAR_IMU imu_sensor;
SOAR_BAROMETER bmu;
void setup(){
  Serial.println(bmu.get_spped_reading());

  Serial.begin(9600);
  imu_sensor.BNO_SETUP();
}
void loop(){

>>>>>>> Stashed changes
}