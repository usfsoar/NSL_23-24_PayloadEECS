#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP3XX.h>

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BMP3XX bmp;  

float previous_altitude = 0;
float current_altitude = 0;

void setup() {
  Serial.begin(115200);

  if (!bmp.begin_I2C()) {
    Serial.println("Could not find a valid BMP3 sensor, check wiring!");
    while (1);
  }

  bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmp.setOutputDataRate(BMP3_ODR_50_HZ);
}


void loop() {
  if (!bmp.performReading()) {
    Serial.println("Failed to perform reading :(");
    return;
  }

  previous_altitude = current_altitude;
  current_altitude = bmp.readAltitude(SEALEVELPRESSURE_HPA);

  altitudeTrigger(current_altitude);

  delay(100);
}

bool altitudeTrigger(float current_altitude) {
  static float previous_altitude = 0;  // Keep track of the previous altitude

  // Check if the altitude is decreasing and above 30.48 meters
  if ((current_altitude - previous_altitude < 0) && current_altitude > 200) {
    if (current_altitude > 30.48) { //100 feet
      return true;  // TRIGGER
    } else {
      return false;  // NOTRIGGER
    }
  } else {
    return false;  // NOTRIGGER
  }

  // Update previous_altitude for the next function call
  previous_altitude = current_altitude;
}
