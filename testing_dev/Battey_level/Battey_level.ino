/*
The Teensy 4.0 operates at 3.3V, and its analog pins are designed to accept a maximum input 
voltage of 3.3V. This is an important difference from many Arduino models that operate at 5V. 
Connecting a voltage higher than 3.3V to an analog pin on the Teensy 4.0 can damage the board.
To read the battery voltage with the Teensy 4.0, especially if the battery voltage is higher 
than 3.3V, you need to use a voltage divider to step down the voltage to a safe level. 
To connect, connect the + terminal to one of the analog pin and connect the - terminal
of battery to GND.
*/

int batteryPin = A0; // The analog pin the battery is connected to

void setup() {
  Serial.begin(9600);
}

void loop() {
  int sensorValue = analogRead(batteryPin);
  float voltage = sensorValue * (3.3 / 4095) * 2; // 5 is valtage, Assuming a 1:1 voltage divider, adjust the multiplier based on your actual divider

  // Assuming a simple linear type battery
  float fullVoltage = 4.2; // Full battery voltage
  float emptyVoltage = 3.2; // Empty battery voltage
  float batteryLevel = map(voltage, emptyVoltage, fullVoltage, 0, 100);
  batteryLevel = constrain(batteryLevel, 0, 100); // Constrain to 0-100%

  Serial.print("Battery Level: ");
  Serial.print(batteryLevel);
  Serial.println("%");

  delay(1000); // Wait for a second for next read
}
