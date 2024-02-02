#include <Arduino.h>  
void setup() {
  Serial.begin(115200);
}

//Every 2 seconds print: <LORA>+RCV=7,31,GPS: V,,,,,0.00,0.00,060180,,,N,-25,11 </LORA>
void loop() {
  Serial.println("<LORA>+RCV=7,31,GPS: V,,,,,0.00,0.00,060180,,,N,-25,11 </LORA>");
  delay(2000);
}