#include "SOAR_gps.h"

SOAR_GPS::SOAR_GPS() : GPSSerial(1), GPS(&GPSSerial) {}

void SOAR_GPS::SOAR_GPS_Setup(){
  GPSSerial.begin(9600, SERIAL_8N1, RX, TX);
  GPS.begin(9600);

  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
  GPS.sendCommand(PGCMD_ANTENNA);
  GPSSerial.println(PMTK_Q_RELEASE);
}
