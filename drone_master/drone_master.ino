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
EmergencyTrigger et(34.4, 60.9); //Critical velocity and height m/s and m
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
bool GetFakeLoraAvailable(){
  //Send a request for lora message
  byte request = 0x08;
  Serial.write(request);
  Serial.write(request);

  uint32_t timeout_start = millis();
  //Check for a rsponse code 0x11 that will come with a boolean value
  while(millis()-timeout_start<1000){
    if(Serial.available()){
      byte responseCode = Serial.read();
      if(responseCode == 0x09){
        //Read the next byte as a boolean
        if(Serial.available()){
          return Serial.read();
        }
      }
    }
  }
  return false;
}
String GetFakeLora(){
  // Send a request for lora
  Serial.write(0x10);
  Serial.write(0x01);
  uint32_t timeout_start = millis();
  // Wait for a string response that wil come in bytes and ended with a null character
  String response = "";
  while (millis() - timeout_start < 1000)
  {
    if(Serial.available()){
      byte responseCode = Serial.read();
      if (responseCode == 0x11) // Arbitrary response code
      {
        response = "";
        byte res_len = Serial.read();
        // Read the next res_len bytes as a string
        for (int i = 0; i < res_len; i++)
        {
          if (Serial.available())
          {
            response += (char)Serial.read();
          }
        }
        break;
      }
    }
  }
  return response;
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

void update_current_sd_file(float *a, float *b, float *c, float *d, float *e, float f, float g, float h){
  String out = String(millis()) + " , " + String(a[0]) + " , " + String(a[1]) + " , " + String(a[2]) + " , " + String(b[0]) + " , " + String(b[1]) + " , " + String(b[2]) + " , " + String(c[0]) + " , " + String(c[1]) + " , " + String(c[2]) + " , " + String(d[0]) + " , " + String(d[1]) + " , " + String(d[2]) + " , " + String(d[3]) + " , " + String(e[0]) + " , " + String(e[1]) + " , " + String(e[2]) + " , " + String(f) + " , " + String(g) + " , " + String(h) + "\n";
#if DEBUG_IMU
  Serial.println(out);
#endif
  const char * ch = out.c_str();
  sd_card.appendFile("/Drone_data.csv", ch);

  return;
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
  bool lora_available;
  #if !DIGITAL_TWIN
  lora_available = lora.available();
  #else
  lora_available = GetFakeLoraAvailable();
  #endif
  if(lora_available){
    String lora_message;
    #if !DIGITAL_TWIN
    lora_message = lora.read();
    #else
    lora_message = GetFakeLora();
    #endif
    if(lora_message == "JET:TRIG"){
      et.jettisonTrigger();
      //reply back with JET:TRIG+RCV
      lora.queueCommand("JET:TRIG+RCV");
    }
    else if(lora_message == "ABT:TRIG"){
      et.abortTrigger();
      //reply back with ABT:TRIG+RCV
      lora.queueCommand("ABT:TRIG+RCV");
    }
  }
}



