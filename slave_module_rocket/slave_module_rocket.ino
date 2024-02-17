// #include <Wire.h>
// #include <avr/wdt.h>
#include <Adafruit_GPS.h>
// #include <SoftwareSerial.h>
#include "ota_update.h"
#include <HardwareSerial.h>
#define RX A4 // Black wire
#define TX A3 // Red wire
uint32_t GPS_FOCUS_MAX = 80000;
#include "buzzer_notify.h"

// GPS Hardware Serial Initiation

OTA_Update otaUpdater("soar-recovery", "L42ARO", "Tron2010");
HardwareSerial GPSSerial(1); // GPS
Adafruit_GPS GPS(&GPSSerial); // GPS

HardwareSerial lora(0); // LoRa

// Transceiver Hardware Serial Initiation
// HardwareSerial lora(RX, TX); // RX, TX --> physically(RX=7, TX=6) 902 mhz band

#define buzzerPin A0
BuzzerNotify buzzerNotify(buzzerPin);


// RRC3
// HardwareSerial rrc3(1);

// #define SLAVE_ADDRESS 0x08

byte data_to_send = 0;
byte data_to_echo = 0;
String output="IDLE";
String command = "";
#define GPSECHO  false



void setup() {

  Serial.begin(115200);
  // lora.begin(115200); // Initialize Software Serial
  // LoRa
  buzzerNotify.Setup();
  lora.begin(115200, SERIAL_8N1, -1, -1);
  sendATcommand("AT+ADDRESS=7", 500);
  sendATcommand("AT+BAND=905000000", 500);
  sendATcommand("AT+NETWORKID=5", 500); 

  // RRC3
  // rrc3.begin(9600, SERIAL_8N1, 1, 0);

  // GPS
  // Pins 4 and 3, but I have to add 1??
  // https://wiki.seeedstudio.com/xiao_esp32s3_pin_multiplexing/#other-hardware-serial
  GPSSerial.begin(9600, SERIAL_8N1, RX, TX);
  GPS.begin(9600);

  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);

  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate

  GPS.sendCommand(PGCMD_ANTENNA);
  // delay(1000);
  // Ask for firmware version
  GPSSerial.println(PMTK_Q_RELEASE);
  
  Serial.println("Setup completed");
  buzzerNotify.Trigger();
  otaUpdater.Setup();
}   

uint32_t timer = millis();
bool gps_focus = true;
uint32_t gps_focus_cycles = 0;

// RRC3
// bool altimeter_focus = false;
// uint32_t altimeter_focus_cycles = 0;
// String old_alti_info = "";

// uint32_t timer_write_test = 5000;
String gpsString = "";
void loop() {
  if(!gps_focus){

    if (command != ""){
      send_command(command);
      command = "";
    }
  
    // lora.listen();  
    String incomingString ="";
    if(lora.available()){
      Serial.print("Request Received: ");
      incomingString = lora.readString();
      delay(50); 
      char dataArray[incomingString.length()];
      incomingString.toCharArray(dataArray, incomingString.length());
      char* data = strtok(dataArray,",");
      data = strtok(NULL,",");
      data = strtok(NULL, ",");
      Serial.println(data);
      String data_str = String(data);
      if (data_str == "GPS"){
        // send_command(gpsString);
        Serial.println("Beginning gps focus");
        gps_focus = true;
        gps_focus_cycles = 0;
      }
      else if(data_str =="PING"){
        output= "PING";
      }
      // else if (data_str=="REBOOT"){
      //   wdt_enable(WDTO_15MS); // Enable watchdog timer with 15ms timeout
      //   while (1) {}
      // }
      // RRC3
      // else if (data_str=="ALTI"){
      //   `
      // }
      else{
        output=data_str;
      }
    }
  }
  if(gps_focus){
    // if(GPS.available()){
      char c = GPS.read();
    if ((c) && (GPSECHO)){
      Serial.write(c);
    }

    if (GPS.newNMEAreceived()) {
      if (!GPS.parse(GPS.lastNMEA())){
        gps_focus_cycles++;
        Serial.println("Failed to parse");
        String gpsString = "GPS: Not Ready";
        Serial.println("GPS: Not Ready");
      }
      else{
        char* gps_data = GPS.lastNMEA();
        String gps_data_string = String(gps_data);
        String vital_gps_info = "GPSRCKT: " + gps_data_string.substring(18,44);
        // String vital_gps_info = "GPS: " + gps_data_string;

        // lora.listen();
        send_command(vital_gps_info);
        Serial.println(gpsString);

        Serial.println();
        Serial.print("Releasing GPS Focus. Took cycles: ");
        Serial.println(gps_focus_cycles);
        gps_focus = false;
        gps_focus_cycles = 0;
      }
    }
    else if (gps_focus_cycles > GPS_FOCUS_MAX){
      gps_focus = false;
      gps_focus_cycles = 0;
      command = "GPS FAIL";
      Serial.println("GPS Focus Timed Out");
    }
    else{
      gps_focus_cycles++;
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
}

void loraSend(const char *toSend, unsigned long milliseconds=500){
  for(int i =0; i<3; i++){
    String res = sendATcommand(toSend, milliseconds);
    Serial.println(res);
    if (res.indexOf("+ERR")>=0){
      Serial.println("Err response detected. Retrying...");
    }
    else{
      break;
    }
  }
}

void send_command(String inputString)
{
  int len = inputString.length();
  Serial.println(inputString);
  char returnedStr[len];
  inputString.toCharArray(returnedStr, len + 1);
  Serial.println(returnedStr);
  if (len <= 9)
  {
    char tempArray[12 + len];
    sprintf(tempArray, "AT+SEND=1,%d,", len);
    strcat(tempArray, returnedStr);
    Serial.println(tempArray);
    loraSend(tempArray, 500);
  }
  else if (len > 9 && len <= 99)
  {
    char tempArray[13 + len];
    sprintf(tempArray, "AT+SEND=1,%d,", len);
    strcat(tempArray, returnedStr);
    Serial.println(tempArray);
    loraSend(tempArray, 500);
  }
  else
  {
    char tempArray[14 + len];
    sprintf(tempArray, "AT+SEND=1,%d,", len);
    strcat(tempArray, returnedStr);
    Serial.println(tempArray);
    loraSend(tempArray, 500);
  }
}

String sendATcommand(const char *toSend, unsigned long milliseconds) {
  String result;
  Serial.print("Sending: ");
  Serial.println(toSend);
  lora.println(toSend);
  unsigned long startTime = millis();
  Serial.print("Received: ");
  while (millis() - startTime < milliseconds) {
    if (lora.available()) {
      char c = lora.read();
      Serial.write(c);
      result += c;  // append to the result string
    }
  }
  Serial.println();  // new line after timeout.
  return result;
}


