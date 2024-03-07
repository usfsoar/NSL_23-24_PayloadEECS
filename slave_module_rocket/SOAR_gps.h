#ifndef SOAR_GPS_h
#define SOAR_GPS_h
#include <Adafruit_GPS.h>

class SOAR_GPS {
  public:
    SOAR_GPS(); // Constructor
    void SOAR_GPS_Setup();
    
    private:
      HardwareSerial GPSSerial;   // GPS
      Adafruit_GPS GPS;  // GPS
    // Declare private variables and methods here
};

#endif