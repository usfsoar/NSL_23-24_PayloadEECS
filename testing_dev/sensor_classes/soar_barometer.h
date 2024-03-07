#ifndef SOAR_BAROMETER_h
#define SOAR_BAROMETER_h

#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"
#define SEALEVELPRESSURE_HPA (1013.25)
#include <Wire.h>

class SOAR_BAROMETER {
  public:
    SOAR_BAROMETER(); // Constructor
    void Initialize();
    float get_speed_reading();
    float * get_dataframe();
    float get_last_altitude_reading();

    // New method prototypes for accessing altitude, pressure, and temperature
    float get_altitude(); // Access altitude
    float get_pressure(); // Access pressure
    float get_temperature(); // Access temperature

    bool descent_check(); // You might also want to implement or declare this method properly if it's needed
    Adafruit_BMP3XX bmp;

  private:
    // Declare private variables and methods here
};

#endif
