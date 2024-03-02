#include "SOAR_BAROMETER.h"



// Implement other methods here
void SOAR_BAROMETER::Initialize(){
  Wire.begin();
<<<<<<< HEAD
  Serial.println("Adafruit BMP388 / BMP390 test");
=======
  Serial.println("Adafruit BMP388 / BMP390 init");
>>>>>>> d1b5cbcbd0037f06ed92ca16f435c46a39709fa1
  if (!this->bmp.begin_I2C())
  { // hardware I2C mode, can pass in address & alt Wire
    // if (! bmp.begin_SPI(BMP_CS)) {  // hardware SPI mode
    // if (! bmp.begin_SPI(BMP_CS, BMP_SCK, BMP_MISO, BMP_MOSI)) {  // software SPI mode
    Serial.println("Could not find a valid BMP3 sensor, check wiring!");
    Serial.println("Sensor setup error");
    return;
  }

  // Set up oversampling and filter initialization
  this->bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  this->bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  this->bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  this->bmp.setOutputDataRate(BMP3_ODR_50_HZ);
}


SOAR_BAROMETER::SOAR_BAROMETER(){
  Serial.print("Barometer Sensor Object created");
  
}


float SOAR_BAROMETER::get_speed_reading() {

  float* floatArray = this->get_dataframe();
  float avg_speed = ((floatArray[4]-floatArray[3])+(floatArray[3]-floatArray[2])+(floatArray[2]-floatArray[1])+(floatArray[1]-floatArray[0]))/4;
  return round(avg_speed*10);
}



float *  SOAR_BAROMETER::get_dataframe (){
   float* floatArray = new float[5];

  floatArray[0] = this->get_last_altitude_reading();
  delay(100);
  floatArray[1] = this->get_last_altitude_reading();
  delay(100);
  floatArray[2] = this->get_last_altitude_reading();
  delay(100);
  floatArray[3] = this->get_last_altitude_reading();
  delay(10);
  return floatArray;
    
}




float SOAR_BAROMETER::get_last_altitude_reading(){
<<<<<<< HEAD
  float bmp_fail=0;
  if (!this->bmp.performReading())
  {
    int bmp_fail;
    Serial.println("Failed to perform reading :(");
    bmp_fail++;
=======
  if (!this->bmp.performReading() && millis()-fail_checkpoint > 10000)
  {
    bmp_fail++;
 


// New methods implementation
float SOAR_BAROMETER::get_altitude() {
  if (!this->bmp.performReading()) {
    Serial.println("Failed to perform reading :(");
    return 0; // Error handling or try again logic could be implemented here
  }
  return this->bmp.readAltitude(SEALEVELPRESSURE_HPA);
}

float SOAR_BAROMETER::get_pressure() {
  if (!this->bmp.performReading()) {
    Serial.println("Failed to perform reading :(");
    return 0; // Error handling or try again logic could be implemented here
  }
  return this->bmp.pressure;
}

float SOAR_BAROMETER::get_temperature() {
  if (!this->bmp.performReading()) {
    Serial.println("Failed to perform reading :(");
    return 0; // Error handling or try again logic could be implemented here
  }
  return this->bmp.temperature;
}   Serial.println("Failed to perform reading :(");
>>>>>>> d1b5cbcbd0037f06ed92ca16f435c46a39709fa1
    if (bmp_fail > 10)
    {
      bmp_fail = 0;
      this->Initialize();
<<<<<<< HEAD
=======
      fail_checkpoint= millis();
>>>>>>> d1b5cbcbd0037f06ed92ca16f435c46a39709fa1
      delay(100);
    }
    // Attempt to reconnect to the sensor
    return 0;
  }
  bmp_fail = 0;
  return this->bmp.readAltitude(SEALEVELPRESSURE_HPA);
}