#include <Wire.h>
#include <SPI.h>
#include "soar_imu.h"
#include "SOAR_SD_CARD.h"
#include "SOAR_BAROMETER.h"
#define DEBUG_IMU false

SOAR_IMU imu_sensor;
SOAR_BAROMETER barometer;
SOAR_SD_CARD sd_card(A1);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  sd_card.begin();
  sd_card.deleteFile( "/Drone_data.csv");
  sd_card.writeFile("/Drone_data.csv", "time, acc_x, acc_y, acc_z, linacc_x, linacc_y, linacc_z, grav_x, grav_y, grav_z, eurl_x, eurl_y, eurl_z, eurl_w, gyro_x, gyro_y, gyro_z, temp, pressure, altitude \n");
  imu_sensor.BNO_SETUP();

}

void loop() {
  float *accel = imu_sensor.GET_ACCELERATION();
  float *linear = imu_sensor.GET_LINEARACCEL();
  float *gravity = imu_sensor.GET_GRAVITY();
  float *quat = imu_sensor.GET_QUAT();
  float *gyro = imu_sensor.GET_GYROSCOPE();

  float temp = barometer.get_temperature();
  float pressure = barometer.get_pressure();
  float altimeter = barometer.get_altitude();


  update_current_sd_file(accel, linear, gravity, quat, gyro, temp, pressure, altimeter);

}

void update_current_sd_file(float *a, float *b, float *c, float *d, float *e, float f, float g, float h){
  String out = String(millis()) + " , " + String(a[0]) + " , " + String(a[1]) + " , " + String(a[2]) + " , " + String(b[0]) + " , " + String(b[1]) + " , " + String(b[2]) + " , " + String(c[0]) + " , " + String(c[1]) + " , " + String(c[2]) + " , " + String(d[0]) + " , " + String(d[1]) + " , " + String(d[2]) + " , " + String(d[3]) + " , " + String(e[0]) + " , " + String(e[1]) + " , " + String(e[2]) + " , " + String(f) + " , " + String(g) + " , " + String(h) + "\n";
#if DEBUG_IMU
  Serial.println(out);
#endif
  const char * ch = out.c_str();
  sd_card.appendFile("/Drone_data.csv", ch);

  return;
}

