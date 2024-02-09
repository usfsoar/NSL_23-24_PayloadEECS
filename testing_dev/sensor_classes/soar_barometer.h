
#ifndef SOAR_BAROMETER_h
#define SOAR_BAROMETER_h
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"
#define SEALEVELPRESSURE_HPA (1013.25)
#include <Wire.h>
class SOAR_BAROMETER {
  public:
    SOAR_BAROMETER(); // Constructor
    // Declare methods and variables here
    void Initialize();
     
    float get_speed_reading();
    float get_last_altitude_reading();
    float * get_dataframe();
    bool descent_check();
    Adafruit_BMP3XX bmp;

  private:
    // Declare private variables and methods here
};

#endif
