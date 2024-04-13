#include "SOAR_gps.h"

SOAR_GPS::SOAR_GPS(int serial_bus, int TX, int RX) : 
  GPSSerial(serial_bus), GPS(&GPSSerial), RX_PIN(RX), TX_PIN(TX){}

void SOAR_GPS::setup(){
  GPSSerial.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
  GPS.begin(9600);

  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
  GPS.sendCommand(PGCMD_ANTENNA);
  GPSSerial.println(PMTK_Q_RELEASE);
}


void SOAR_GPS::GET_NMEA(char* nmea, bool& ready, bool& failed){

  char read = GPS.read();
  if(!GPS.newNMEAreceived()){
    *ready = false;
    return;
  }
  if (!GPS.parse(GPS.lastNMEA())){
    *failed = true;
    return;
  }

  char* gps_data = GPS.lastNMEA();

  char *p;
  int comas = 0;
  for(p=gps_data; *p!='\0'; p++){
    if(*p == ','){
      comas++;
    }
    if(comas < 6){
      continue;
    }elif(comas > 10){
      break;
    }
    *(nmea++) = *p;
  }
  

}

  //    if (millis() - gps_repeat_focus_checkpoint > GPS_FOCUS_MAX) {
  //     // gps_repeat_focus = false;
  //     gps_repeat_focus_checkpoint = millis();
  //     lora.stringPacketWTime("GSFI");
  //     Serial.println("GPS Repeat Focus Timed Out");


