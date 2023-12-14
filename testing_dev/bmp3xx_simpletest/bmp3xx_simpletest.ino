#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"

#define BMP_SCK 13
#define BMP_MISO 12
#define BMP_MOSI 11
#define BMP_CS 10

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BMP3XX bmp;
uint32_t cycles = 0;
int MCycles = 0;

void bmp_setup() {
  Wire.begin(13, 12);
  while (!Serial);
  Serial.println("Adafruit BMP388 / BMP390 test");
  if (!bmp.begin_I2C()) {   // hardware I2C mode, can pass in address & alt Wire
    //if (! bmp.begin_SPI(BMP_CS)) {  // hardware SPI mode  
    //if (! bmp.begin_SPI(BMP_CS, BMP_SCK, BMP_MISO, BMP_MOSI)) {  // software SPI mode
      Serial.println("Could not find a valid BMP3 sensor, check wiring!");
      Serial.println("Sensor setup error");
      return;
    }

    // Set up oversampling and filter initialization
    bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
    bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
    bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
    bmp.setOutputDataRate(BMP3_ODR_50_HZ);
}

void setup() {
  Serial.begin(115200);
  bmp_setup();
}
float GetAltitude(){
  if (!bmp.performReading()) {
    Serial.println("Failed to perform reading :(");
    MCycles = 0;
    cycles = 0;
    // Attempt to reconnect to the sensor
    bmp_setup();
    return 0;
  }
  return bmp.readAltitude(SEALEVELPRESSURE_HPA);
}
void loop() {
  float alt = GetAltitude();
  Serial.println(alt);
  delay(50);
  // if (!bmp.performReading()) {
  //   Serial.println("Failed to perform reading :(");
  //   MCycles = 0;
  //   cycles = 0;
  //   // Attempt to reconnect to the sensor
  //   bmp_setup();
  //   return 0;
  // }
  // Serial.print("Cycles with no issue (MCycles, cycles):");
  // Serial.print(MCycles);
  // Serial.print(",");
  // Serial.println(cycles);
  // Serial.print("Temperature = ");
  // Serial.print(bmp.temperature);
  // Serial.println(" *C");

  // Serial.print("Pressure = ");
  // Serial.print(bmp.pressure / 100.0);
  // Serial.println(" hPa");

  // Serial.print("Approx. Altitude = ");
  // Serial.print(bmp.readAltitude(SEALEVELPRESSURE_HPA));
  // Serial.println(" m");

  // Serial.println();
  // cycles ++;
  // if (cycles >= UINT32_MAX - 1000) {
  //   // Increase MCycles
  //   MCycles++;
    
  //   // Reset cycles
  //   cycles = 0;
  // }
  // delay(50);
}
