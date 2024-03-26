#include "_config.h"
#include <Wire.h>
// #include <SPI.h>
#include "soar_imu.h"
#include "SOAR_SD_CARD.h"
#include "ota_update.h"
#define RX A2  // Black wire
#define TX A3  // Red wire
uint32_t GPS_FOCUS_MAX = 10000;
#include "buzzer_notify.h"
#include <queue>
#include "SOAR_Lora.h"
#include <Adafruit_GPS.h>
#include <HardwareSerial.h>
#include "utils.h"
#define DEBUG_IMU false

// #include <avr/wdt.h>
// #include <SoftwareSerial.h>

// Create SOAR_IMU instance
SOAR_IMU imu_sensor;

// Create sd class instance
SOAR_SD_CARD sd_card(A1);

// GPS Hardware Serial Initiation

OTA_Update otaUpdater("soar-recovery", "TP-Link_BCBD", "10673881");
HardwareSerial GPSSerial(1);   // GPS
Adafruit_GPS GPS(&GPSSerial);  // GPS
SOAR_Lora lora("7", "5", "433000000");  // LoRa

// Transceiver Hardware Serial Initiation
// HardwareSerial lora(RX, TX); // RX, TX --> physically(RX=7, TX=6) 902 mhz band

#define buzzerPin A0
BuzzerNotify buzzerNotify(buzzerPin);


// RRC3
// HardwareSerial rrc3(1);

// #define SLAVE_ADDRESS 0x08

byte data_to_send = 0;
byte data_to_echo = 0;
String output = "IDLE";
#define GPSECHO false



void setup() {

  Serial.begin(115200);
  // lora.begin(115200); // Initialize Software Serial
  // LoRa
  buzzerNotify.Setup();
  lora.begin();

  // RRC3
  // rrc3.begin(9600, SERIAL_8N1, 1, 0);

  // GPS
  // Pins 4 and 3, but I have to add 1??
  // https://wiki.seeedstudio.com/xiao_esp32s3_pin_multiplexing/#other-hardware-serial
  GPSSerial.begin(9600, SERIAL_8N1, RX, TX);
  GPS.begin(9600);

  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);

  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);  // 1 Hz update rate

  GPS.sendCommand(PGCMD_ANTENNA);
  // delay(1000);
  // Ask for firmware version
  GPSSerial.println(PMTK_Q_RELEASE);

  Serial.println("Setup completed");
  buzzerNotify.Trigger();
  otaUpdater.Setup();

  lora.stringPacketWTime("WU",6);


  sd_card.begin();
  sd_card.deleteFile( "/imu_data.csv");
  sd_card.writeFile("/imu_data.csv", "time, accel_x, accel_y, accel_z, linear_x, linear_y, linear_z, gravity_x, gravity_y, gravity_z, quat_w, quat_x, quat_y, quat_z, gyro_x, gyro_y, gyro_z\n");
  // sd_card.writeFile("/imu_data.csv", "time, accel_x, accel_y, accel_z\n");
  imu_sensor.BNO_SETUP();
}

bool gps_focus = false;
bool gps_repeat_focus = false;
uint32_t gps_focus_checkpoint = 0;
uint32_t gps_repeat_focus_checkpoint = 0;

// RRC3
// bool altimeter_focus = false;
// uint32_t altimeter_focus_cycles = 0;
// String old_alti_info = "";

// uint32_t timer_write_test = 5000;
String gpsString = "";


// char command1[] = "hello";
// char command2[] = "bonjour";
// char command3[] = "guten tag";



void loop() {
  // if (!gps_focus) {

  //   // lora.listen();
  //   String incomingString = "";
  //   if (lora.available()) {
  //     String data_str = lora.read();
  //     if (data_str == "GPS:SINGLE") {
  //       Serial.println("Beginning gps focus");
  //       lora.queueCommand("GPS:SINGLE+RCV");
  //       gps_focus = true;
  //       gps_focus_checkpoint = millis(); 

  //       // Silence the repeat focus
  //       gps_repeat_focus = false;
  //       gps_repeat_focus_checkpoint = millis();
  //     } 
  //     else if(data_str=="GPS:REPEAT"){
  //       lora.queueCommand("GPS:REPEAT+RCV");

  //       // Tell repeat focus to start talking
  //       gps_repeat_focus = true;
  //       gps_repeat_focus_checkpoint = millis();
  //     }
  //     else if (data_str == "PING") {
  //       lora.queueCommand("PONG");

  //       // Silence the repeat focus
  //       gps_repeat_focus = true;
  //       gps_repeat_focus_checkpoint = millis();
  //     } 
  //     else if(data_str == "IMU"){
  //       float *a = imu_sensor.GET_ACCELERATION();
  //       String out =  "IMU:"+String(millis()) + " , " + String(a[0]) + " , " + String(a[1]) + " , " + String(a[2]);
  //       lora.queueCommand(out);
  //     }
  //     else {
  //       lora.queueCommand("INVALID:"+data_str);

  //       // Silence the repeat focus
  //       gps_repeat_focus = true;
  //       gps_repeat_focus_checkpoint = millis();
  //     }
  //   }
  // }   
  // if (gps_focus) {
  //   char c = GPS.read();
  //   if ((c) && (GPSECHO)) {
  //     Serial.write(c);
  //   }

  //   if (GPS.newNMEAreceived()) {
  //     if (!GPS.parse(GPS.lastNMEA())) {
  //       Serial.println("Failed to parse");
  //       String gpsString = "GPS: Not Ready";
  //       Serial.println("GPS: Not Ready");
  //     } else {
  //       char* gps_data = GPS.lastNMEA();
  //       String gps_data_string = String(gps_data);
  //       String vital_gps_info = "GPS:" + gps_data_string.substring(18, 44);
  //       // String vital_gps_info = "GPS: " + gps_data_string;

  //       // lora.listen();
  //       lora.queueCommand(vital_gps_info);
  //       Serial.println(gps_data_string);

  //       Serial.println();
  //       Serial.print("Releasing GPS Focus. Took time: ");
  //       Serial.println(millis() - gps_focus_checkpoint);
  //       gps_focus = false;
  //       gps_focus_checkpoint = millis();
  //     }
  //   }
  //   if (millis() - gps_focus_checkpoint > GPS_FOCUS_MAX) {
  //     gps_focus = false;
  //     gps_focus_checkpoint = millis();
  //     lora.queueCommand("GPS:FAIL");
  //     Serial.println("GPS Focus Timed Out");
  //   }
  // }
  // if (gps_repeat_focus) {
  //   char c = GPS.read();
  //   if ((c) && (GPSECHO)) {
  //     Serial.write(c);
  //   }

  //   if (GPS.newNMEAreceived()) {
  //     if (!GPS.parse(GPS.lastNMEA())) {
  //       Serial.println("Failed to parse");
  //       String gpsString = "GPS: Not Ready";
  //       Serial.println("GPS: Not Ready");
  //     } else {
  //       char* gps_data = GPS.lastNMEA();
  //       String gps_data_string = String(gps_data);
  //       String vital_gps_info = "GPS:" + gps_data_string.substring(18, 44);
  //       // String vital_gps_info = "GPS: " + gps_data_string;

  //       // lora.listen();
  //       lora.queueCommand(vital_gps_info);
  //       Serial.println(gps_data_string);

  //       Serial.println();
  //       Serial.print("Releasing GPS Focus. Took time: ");
  //       Serial.println(millis() - gps_focus_checkpoint);
  //       gps_repeat_focus = true;
  //       gps_repeat_focus_checkpoint = millis();
  //     }
  //   }
  //   if (millis() - gps_repeat_focus_checkpoint > GPS_FOCUS_MAX) {
  //     // gps_repeat_focus = false;
  //     gps_repeat_focus_checkpoint  = millis();
  //     lora.queueCommand("GPS:FAIL");
  //     Serial.println("GPS Focus Timed Out");
  //   }
  // }

  
  int address, length, rssi, snr;
  byte *data;
  bool lora_available = lora.read(&address, &length, &data, &rssi, &snr);
  if (lora_available && length > 0 && lora.checkChecksum(data, length)) // A command is typically 2 bytes
  {
    /*
      |Command|Definition|Response|Definition|
|---|---|---|---|
|`PI`|Ping|`PO{T-time}`|Pong with time|
|`AS`|Altitude Single|`AS{T-time}{Altitude-float}`|Altitude Single Data with time|
|`AR`|Altitude Repeat|`AR{T-time}:R`|Altitude Repeat Received with time|
|`AW`|Write altitude thresholds|`AW{T-time}:R`|Thresholds written with time|
|||`TW{T-time}:F`|Failed to set thresholds with time|
|`AT`|Get altitude thresholds|`AT{T-time}{H1-float}{H2-float}{H3-float}`|Thresholds data with time|
|`DP`|Deploy|`DP{T-time}:R`|Deploy Received with time|
|`DS`|Deploy Status|`DS{T-time}{Status-uint8_t}`|Deploy Status Data with time|
|`DR`|Deploy Status Repeat|`DR{T-time}:R`|Deploy Status Repeat Received with time|
|`DT`|Deploy Stop|`DT{T-time}:R`|Deploy Stop Received with time|
|`DR`|Deploy Reset|`DR{T-time}:R`|Deploy Reset Received with time|
|`DC`|Deploy Retract|`DC{T-time}:R`|Deploy Retract Received with time|
|`LI`|Distance sensor|`LI{T-time}{Distance-uint16_t}`|Distance sensor data with time|
|`LR`|Distance sensor repeat|`LR{T-time}:R`|Distance sensor repeat received with time|
|`IS`|All info single|`IS{T-time}{Altitude-float}{Distance-uint16_t}{Status-uint8_t}`|All info data with time|
|`IR`|All info repeat|`IR{T-time}:R`|All info repeat received with time|
|`JF`|Jog Forward|`JF{T-time}:R`|Jog Forward Received with time|
|`JR`|Jog Reverse|`JR{T-time}:R`|Jog Reverse Received with time|
|`RS`|Stop any repeating data|`RS{T-time}:R`|Stop any repeating data received with time|
|`NH`|Not handled (n bytes)|`NH{T-time}{Command-nB}`|Not handled command with time|
    */
    bool valid_command = true;
    if (length > 2) {
      char command[3] = {data[0], data[1], '\0'};
      if(!strcmp(command, "PI")){
        lora.stringPacketWTime("PO",6);
      }
      else if(!strcmp(command, "GR")){
        lora.stringPacketWTime("GACK",1);
      }
      else if(!strcmp(command, "IR")){
        lora.stringPacketWTime("IR",6);
        if(length >= 8){
          uint32_t freq = Utils::bytesToUint32(&data[2]);
          // autoTelemetry.setRate(freq);
        }
        // autoTelemetry.SetRepeatStatus(1);
      }
      else{
        valid_command = false;
      }
    }else{
      valid_command = false;
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



  // RRC3
  // if(altimeter_focus){
  //   // altimeter.listen();
  //   char d = altimeter.read();
  //   if (old_alti_info != d) {
  //     char* altimeter_data = d;
  //     String altimeter_data_string = String(altimeter_data);
  //     String vital_altimeter_info = "Alti: " + altimeter_data_string;

  //     // lora.listen();
  //     send_command(vital_altimeter_info);
  //   }
  // }
  buzzerNotify.Check();
  otaUpdater.Handle();
  lora.handleQueue();

  float *accel = imu_sensor.GET_ACCELERATION();
  float *linear = imu_sensor.GET_LINEARACCEL();
  float *gravity = imu_sensor.GET_GRAVITY();
  float *quat = imu_sensor.GET_QUAT();
  float *gyro = imu_sensor.GET_GYROSCOPE();
  update_current_sd_file(accel, linear, gravity, quat, gyro);
  delete[] accel;
  delete[] linear;
  delete[] gravity;
  delete[] quat;
  delete[] gyro;

}


void update_current_sd_file(float *a, float *l, float *g, float *q, float *gyro){
  // String out =  String(millis()) + " , " + String(a[0]) + " , " + String(a[1]) + " , " + String(a[2]) +"\n";
  //Include all the data in the file
  String out =  String(millis()) + " , " + String(a[0]) + " , " + String(a[1]) + " , " + String(a[2]) + " , " + String(l[0]) + " , " + String(l[1]) + " , " + String(l[2]) + " , " + String(g[0]) + " , " + String(g[1]) + " , " + String(g[2]) + " , " + String(q[0]) + " , " + String(q[1]) + " , " + String(q[2]) + " , " + String(q[3]) + " , " + String(gyro[0]) + " , " + String(gyro[1]) + " , " + String(gyro[2]) +"\n";
#if DEBUG_IMU
  Serial.println(out);
#endif
  const char * c = out.c_str();
  sd_card.appendFile("/imu_data.csv", c);

  return;
}


// class AutomatedTelemetry
// {
//   private:
//     int _repeat_status = 0; // 0: no repeat, 1: repeat all, 2: repeat altitude, 3: repeat distance, 4: repeat status
//     uint32_t _last_repeat = 0;
//     uint32_t _repeat_interval = 1000;
//   public:
//     AutomatedTelemetry(uint32_t repeat_interval){
//       _repeat_interval = repeat_interval;
//     }
//     void SetRepeatStatus(int status){
//       _repeat_status = status;
//     }
//     void Handle(float *accel,float *gyro,float temp,float pressure){
//       if(_repeat_status == 0) return;
//       //Check for repeat interval
//       if(millis()-_last_repeat < _repeat_interval) return;
//       switch(_repeat_status){
//         case 1:
//           //repeat stemnaut data
//           lora.beginPacket();
//           lora.sendChar("SR");
//           lora.sendFloat(accel[0]);
//           lora.sendFloat(accel[1]);
//           lora.sendFloat(accel[2]);
//           lora.sendFloat(gyro[0]);
//           lora.sendFloat(gyro[1]);
//           lora.sendFloat(gyro[2]);
//           lora.sendFloat(temp);
//           lora.sendFloat(0.0); //supposed to return sound value as a float, but we don't have that yet
//           lora.sendFloat(pressure);
//           lora.endPacketWTime(6);
//           break;
//         case 2:
//           //!!Put all of the GPS related data that we'd want to call a repeat command for
//           break;
//         default:
//           break;
//       }
//       _last_repeat = millis();
//     }
// };