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

}
