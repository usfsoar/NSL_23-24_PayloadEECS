#include "_config.h"
#include <Wire.h>
#include <SPI.h>
#include "soar_imu.h"
#include "SOAR_SD_CARD.h"
#include "SOAR_BAROMETER.h"
#include "emergency_trigger.h"
#include "soar_speaker.h"
#include <PWMServo.h>
#include "SOAR_Lora.h"
#include <math.h>  
#include <cmath>
#define PARACHUTE_SERVO_PIN 9 
// #define JETTISON1_SERVO_PIN 5 //PIN NUMBER SUBJECT TO CHANGE. It was set to 10 for placeholding purposes
// #define JETTISON2_SERVO_PIN 9 //Change this one

#define DEBUG_IMU false
//create servo object to control the servo

PWMServo parachuteServo;
// PWMServo jettisonServo1;
// PWMServo jettisonServo2;
SOAR_IMU imu_sensor;
SOAR_BAROMETER barometer;
SOAR_SD_CARD sd_card(10);
EmergencyTrigger et(34.4, 60.9); //Critical velocity and height m/s and m
SOAR_Lora lora("10", "5", "433000000",1000);
SOAR_Speaker speaker;


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

class AutomatedTelemetry
{
  private:
    int _repeat_status = 0; // 0: no repeat, 1: repeat all, 2: repeat altitude, 3: repeat distance, 4: repeat status
    uint32_t _last_repeat = 0;
    uint32_t _repeat_interval = 1000;
  public:
    AutomatedTelemetry(uint32_t repeat_interval){
      _repeat_interval = repeat_interval;
    }
    void SetRepeatStatus(int status){
      _repeat_status = status;
    }
    void Handle(float *accel,float *gyro,float temp,float pressure, float altitude){
      if(_repeat_status == 0) return;
      //Check for repeat interval
      if(millis()-_last_repeat < _repeat_interval) return;
      switch(_repeat_status){
        case 1:
          //repeat stemnaut data
          lora.beginPacket();
          lora.sendChar("SR");
          lora.sendFloat(accel[0]);
          lora.sendFloat(accel[1]);
          lora.sendFloat(accel[2]);
          lora.sendFloat(gyro[0]);
          lora.sendFloat(gyro[1]);
          lora.sendFloat(gyro[2]);
          lora.sendFloat(temp);
          lora.sendFloat(0.0); //supposed to return sound value as a float, but we don't have that yet
          lora.sendFloat(pressure);
          lora.endPacketWTime(6);
          break;
        case 2:
          //!!Put all of the GPS related data that we'd want to call a repeat command for
          break;
        case 3:
          lora.beginPacket();
          lora.sendChar("AP");
          lora.sendFloat(altitude);
          lora.sendFloat(pressure);
          lora.endPacketWTime(6);
          break;
        default:
          break;
      }
      _last_repeat = millis();
    }
};
AutomatedTelemetry autoTelemetry(500);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  sd_card.begin();
  sd_card.deleteFile( "/Drone_data.csv");
  sd_card.writeFile("/Drone_data.csv", "time, acc_x, acc_y, acc_z, linacc_x, linacc_y, linacc_z, grav_x, grav_y, grav_z, eurl_x, eurl_y, eurl_z, eurl_w, gyro_x, gyro_y, gyro_z, velocity_x, velocity_y, velocity_z, temp, pressure, altitude \n");
  imu_sensor.BNO_SETUP();
  barometer.Initialize();
  parachuteServo.attach(PARACHUTE_SERVO_PIN);
  parachuteServo.write(70);
  //jettisonServo1.attach(JETTISON1_SERVO_PIN);
  // jettisonServo2.attach(JETTISON2_SERVO_PIN);
  lora.begin(3);
  speaker.playMario();
  autoTelemetry.SetRepeatStatus(3);

}

void update_current_sd_file(float *a, float *b, float *c, float *d, float *e, float *f, float g, float h, float i){
  String out = String(millis()) + "," + String(a[0]) + "," + String(a[1]) + "," + String(a[2]) + "," + String(b[0]) + "," + String(b[1]) + "," + String(b[2]) + "," + String(c[0]) + "," + String(c[1]) + "," + String(c[2]) + "," + String(d[0]) + "," + String(d[1]) + "," + String(d[2]) + "," + String(d[3]) + "," + String(e[0]) + "," + String(e[1]) + "," + String(e[2]) + "," + String(f[0]) + "," + String(f[1]) + "," + String(f[2]) + "," + String(g) + "," + String(h) + "," + String(i) + "\n";
#if DEBUG_IMU
  Serial.println(out);
#endif
  
  const char * ch = out.c_str();
  sd_card.appendFile("/Drone_data.csv", ch);

  return;
}


void loop() {
  imu_sensor.loop_iterations++;
  float *accel = imu_sensor.GET_ACCELERATION();
  float *linear = imu_sensor.GET_LINEARACCEL();
  float *gravity = imu_sensor.GET_GRAVITY();
  float *quat = imu_sensor.GET_QUAT();
  float *gyro = imu_sensor.GET_GYROSCOPE();

  float temp = barometer.get_temperature();
  float pressure = barometer.get_pressure();
  float altimeter = barometer.get_altitude();

  float *velocity = imu_sensor.GET_VELOCITY();
  // String out = "x:" + String(velocity[0]) + ",y:" + String(velocity[1]) + ",z:" + String(velocity[2]) + "\n";
  // Serial.print(out);

  update_current_sd_file(accel, linear, gravity, quat, gyro, velocity, temp, pressure, altimeter);

  float velocity_mag = sqrt(pow(double(velocity[0]), 2) + pow(double(velocity[1]), 2) + pow(double(velocity[2]), 2));

  et.checkState(velocity_mag, altimeter);

  int state = et.state;
  // if(state == 2){//Engage emergency parachute
  //   parachuteServo.write(180);
  // }
  
  int address, length, rssi, snr;
  byte *data;
  bool lora_available = lora.read(&address, &length, &data, &rssi, &snr);
  if (lora_available && length > 0 && lora.checkChecksum(data, length)) // A command is typically 2 bytes
  {
    bool valid_command = true;
    if (length > 2){
      char command[3] = {data[0], data[1], '\0'};
      if(!strcmp(command, "PI")){
        lora.stringPacketWTime("PO",6);//reply func
      }else if(!strcmp(command, "AB")){
        parachuteServo.write(180); //activate parachute servo to deploy
        et.abortTrigger();
        lora.stringPacketWTime("AB",6);
      }else if(!strcmp(command, "JL")){
        // jettisonServo1.write(0);
        // jettisonServo2.write(0);
        lora.stringPacketWTime("JL",6);
      }else if(!strcmp(command, "JT")){
        // jettisonServo1.write(90);
        // jettisonServo2.write(90);
        et.jettisonTrigger();  //update state machine
        lora.stringPacketWTime("JT",6);
      }else if(!strcmp(command, "SS")){
        lora.beginPacket();
        lora.sendChar("SS");
        lora.sendFloat(accel[0]);
        lora.sendFloat(accel[1]);
        lora.sendFloat(accel[2]);
        lora.sendFloat(gyro[0]);
        lora.sendFloat(gyro[1]);
        lora.sendFloat(gyro[2]);
        lora.sendFloat(temp);
        lora.sendFloat(0.0); //supposed to return sound value as a float, but we don't have that yet
        lora.sendFloat(pressure);
        lora.endPacketWTime(6);
      }else if (!strcmp(command, "SR")){
        lora.stringPacketWTime("SR",6);
        autoTelemetry.SetRepeatStatus(1);//repeat all Stemnaut data
      }else if(!strcmp(command, "GS")){
        lora.stringPacketWTime("GS",6);
        //add GPS string to send
      }else if (!strcmp(command, "GR")){
        lora.stringPacketWTime("GR",6);
        autoTelemetry.SetRepeatStatus(2);
      }else{
        valid_command=false;
      }
    }else{
      valid_command=false;
    }
    if(!valid_command){
      lora.beginPacket();
      lora.sendChar("NH");
      for (int i = 0; i < length; i++) {
        lora.sendByte(data[i]);
      }
      lora.endPacketWTime(6);
    }
  }
  autoTelemetry.Handle(accel,gyro,temp,pressure,altimeter);
  lora.handleQueue();
  delete[] accel;
  delete [] linear;
  delete[] gravity;
  delete [] quat;
  delete[] gyro;
  delete [] velocity;

}



