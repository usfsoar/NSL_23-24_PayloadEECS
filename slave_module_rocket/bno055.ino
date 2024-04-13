#include <Wire.h> // Include the Wire library for I2C communication
#include <Adafruit_BNO055.h> // Include the Adafruit BNO055 library for BNO055 sensor

#define BNO055_I2C_ADDR 0x28 // BNO055 I2C address
#define SDA_PIN 10 // Define SDA pin
#define SCL_PIN 9 // Define SCL pin

Adafruit_BNO055 bno = Adafruit_BNO055(); // Create a BNO055 object

void setup() {
  Serial.begin(115200); // Initialize serial communication for debugging
  Serial.print("ciao");
  Wire.begin(SDA_PIN, SCL_PIN); // Initialize I2C communication with specified pins

  if (!bno.begin()) {
    Serial.println("Couldn't find BNO055 sensor, check wiring or I2C address!");
    while (1); // Hang here if BNO055 is not found
  }

  bno.setExtCrystalUse(true); // Use external crystal for better accuracy
}

void loop() {
  // Read orientation data
  sensors_event_t orientationData;
  bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);

  // Print orientation data
  Serial.print("Orientation: ");
  Serial.print(orientationData.orientation.x);
  Serial.print(" ");
  Serial.print(orientationData.orientation.y);
  Serial.print(" ");
  Serial.println(orientationData.orientation.z);

  delay(100); // Delay between readings
}
