#include "_config.h"
#include <Wire.h>
#include <SPI.h>
#include "soar_imu.h"
#include "SOAR_SD_CARD.h"
#include "SOAR_BAROMETER.h"
#include "emergency_trigger.h"
#include "servo.h"
#include "SOAR_Lora.h"
#define PARACHUTE_SERVO_PIN 9

#define DEBUG_IMU false
//create servo object to control the servo
Servo parachuteServo;
SOAR_IMU imu_sensor;
SOAR_BAROMETER barometer;
SOAR_SD_CARD sd_card(10);
EmergencyTrigger et(60.0, 60.0);
SOAR_Lora lora("10", "5", "433000000");

#if DIGITAL_TWIN
float GetFakeVelocity()
{
  Serial.write(0x01);
  Serial.write(0x01);
  //Read the serial with a timeout of 1 second for a a response of 3 bytes
  uint32_t start = millis();
  while (millis() - start < 1000) {
    if (Serial.available() >= 3) {
      //Read the first byte
      uint8_t header = Serial.read();
      //Check if the header is correct
      if (header == 0x02) {
        //Read the next two bytes
        uint8_t len = Serial.read();
        uint8_t vel = Serial.read();
        //Return the velocity
        return vel;
      }
    }
  }
  return 0;

}
#endif

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  sd_card.begin();
  sd_card.deleteFile( "/Drone_data.csv");
  sd_card.writeFile("/Drone_data.csv", "time, acc_x, acc_y, acc_z, linacc_x, linacc_y, linacc_z, grav_x, grav_y, grav_z, eurl_x, eurl_y, eurl_z, eurl_w, gyro_x, gyro_y, gyro_z, temp, pressure, altitude \n");
  imu_sensor.BNO_SETUP();
  barometer.Initialize();
  parchuteServo.attach(PARACHUTE_SERVO_PIN);
  lora.begin();

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
  float velocity;
  #if !DIGITAL_TWIN
  velocity = imu_sensor.GET_ACCELERATION()[1];//Here is where custom velocity function should be called
  #else
  velocity = GetFakeVelocity();
  #endif
  et.checkState(velocity, altimeter);
  int state = et.state;
  if(state == 2){//Engage emergency parachute
    parachuteServo.write(90);
  }
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

