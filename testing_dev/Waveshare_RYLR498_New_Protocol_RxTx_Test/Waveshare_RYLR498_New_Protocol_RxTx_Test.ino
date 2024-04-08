#include "_config.h"
#include "ota_update.h"
#define RX A2  // Black wire

#include "buzzer_notify.h"
#include <queue>
#include "SOAR_Lora.h"
#include <HardwareSerial.h>
#include "utils.h"
#define DEBUG_IMU false


OTA_Update otaUpdater("soar-recovery", "TP-Link_BCBD", "10673881");
SOAR_Lora lora("7", "5", "490000000");  // LoRa

#define buzzerPin A0
BuzzerNotify buzzerNotify(buzzerPin);


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
  Serial.println("Setup completed");
  buzzerNotify.Trigger();
  otaUpdater.Setup();

  lora.stringPacketWTime("WU",6);

}



void loop() {
 
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



}

