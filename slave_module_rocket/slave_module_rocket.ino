// #include <Wire.h>
// #include <avr/wdt.h>
#include <Adafruit_GPS.h>
// #include <SoftwareSerial.h>
#include "ota_update.h"
#include <HardwareSerial.h>
#define RX A2  // Black wire
#define TX A3  // Red wire
uint32_t GPS_FOCUS_MAX = 10000;
#include "buzzer_notify.h"
#include <queue>
#include "SOAR_Lora.h"
#include "soar_imu.h"
#include "SOAR_SD_CARD.h"

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

  lora.sendCommand("AWAKE");

  sd_card.begin();
  sd_card.deleteFile( "/imu_data.csv");
  sd_card.writeFile("/imu_data.csv", "time, accel_x, accel_y, accel_z\n");
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
  if (!gps_focus) {

    // lora.listen();
    String incomingString = "";
    if (lora.available()) {
      String data_str = lora.read();
      if (data_str == "GPS:SINGLE") {
        Serial.println("Beginning gps focus");
        lora.queueCommand("GPS:SINGLE+RCV");
        gps_focus = true;
        gps_focus_checkpoint = millis(); 

        // Silence the repeat focus
        gps_repeat_focus = true;
        gps_repeat_focus_checkpoint = millis();
      } 
      else if(data_str=="GPS:REPEAT"){
        lora.queueCommand("GPS:REPEAT+RCV");

        // Tell repeat focus to start talking
        gps_repeat_focus = true;
        gps_repeat_focus_checkpoint = millis();
      }
      else if (data_str == "PING") {
        lora.queueCommand("PONG");

        // Silence the repeat focus
        gps_repeat_focus = true;
        gps_repeat_focus_checkpoint = millis();
      } 
      else if(data_str == "IMU"){
        float *a = imu_sensor.GET_ACCELERATION();
        String out =  "IMU:"+String(millis()) + " , " + String(a[0]) + " , " + String(a[1]) + " , " + String(a[2]);
        lora.queueCommand(out);
      }
      else {
        lora.queueCommand("INVALID:"+data_str);

        // Silence the repeat focus
        gps_repeat_focus = true;
        gps_repeat_focus_checkpoint = millis();
      }
    }
  }
  if (gps_focus) {
    char c = GPS.read();
    if ((c) && (GPSECHO)) {
      Serial.write(c);
    }

    if (GPS.newNMEAreceived()) {
      if (!GPS.parse(GPS.lastNMEA())) {
        Serial.println("Failed to parse");
        String gpsString = "GPS: Not Ready";
        Serial.println("GPS: Not Ready");
      } else {
        char* gps_data = GPS.lastNMEA();
        String gps_data_string = String(gps_data);
        String vital_gps_info = "GPS:" + gps_data_string.substring(18, 44);
        // String vital_gps_info = "GPS: " + gps_data_string;

        // lora.listen();
        lora.queueCommand(vital_gps_info);
        Serial.println(gps_data_string);

        Serial.println();
        Serial.print("Releasing GPS Focus. Took time: ");
        Serial.println(millis() - gps_focus_checkpoint);
        gps_focus = false;
        gps_focus_checkpoint = millis();
      }
    }
    if (millis() - gps_focus_checkpoint > GPS_FOCUS_MAX) {
      gps_focus = false;
      gps_focus_checkpoint = millis();
      lora.queueCommand("GPS:FAIL");
      Serial.println("GPS Focus Timed Out");
    }
  }
  if (gps_repeat_focus) {

    char * commands[] = 
    {
      (char*) "PING",
      (char*) "PONG",
      (char*) "GPS",
      (char*) "GPS:repeat"
    };
    // https://forum.arduino.cc/t/how-to-handle-arrays-of-char-arrays/957534/6
    String incomingString = "";
    if (lora.available()) {
      String data_str = lora.read();
      int length_of_command_list = sizeof(commands) / sizeof(commands[0]);
      for(unsigned i = 0; i < length_of_command_list; i++) {
        if (data_str == commands[i]) {
          gps_repeat_focus = false;
          break;
        }
      }
    }



    char c = GPS.read();
    if ((c) && (GPSECHO)) {
      Serial.write(c);
    }

    if (GPS.newNMEAreceived()) {
      if (!GPS.parse(GPS.lastNMEA())) {
        Serial.println("Failed to parse");
        String gpsString = "GPS: Not Ready";
        Serial.println("GPS: Not Ready");
      } else {
        char* gps_data = GPS.lastNMEA();
        String gps_data_string = String(gps_data);
        String vital_gps_info = "GPS:" + gps_data_string.substring(18, 44);
        // String vital_gps_info = "GPS: " + gps_data_string;

        // lora.listen();
        lora.queueCommand(vital_gps_info);
        Serial.println(gps_data_string);

        Serial.println();
        Serial.print("Releasing GPS Focus. Took time: ");
        Serial.println(millis() - gps_focus_checkpoint);
        gps_repeat_focus = true;
        gps_repeat_focus_checkpoint = millis();
      }
    }
    if (millis() - gps_focus_checkpoint > GPS_FOCUS_MAX) {
      gps_repeat_focus = false;
      gps_repeat_focus_checkpoint  = millis();
      lora.queueCommand("GPS:FAIL");
      Serial.println("GPS Focus Timed Out");
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
  Serial.println("SOAR-DEPLOY");

  float *accel = imu_sensor.GET_ACCELERATION();
  update_current_sd_file(accel);

}


void update_current_sd_file(float *a){
  String out =  String(millis()) + " , " + String(a[0]) + " , " + String(a[1]) + " , " + String(a[2]) +"\n";
  const char * c = out.c_str();
  sd_card.appendFile("/imu_data.csv", c);

  return;
}


