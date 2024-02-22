#include <SoftwareSerial.h>

SoftwareSerial mySerial(9,8);
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  mySerial.begin(9600);
  // pinMode(10, OUTPUT);
  Serial.println("Setup complete");
}

void loop() {
  // put your main code here, to run repeatedly:
  mySerial.listen();
  if(!mySerial.available()){
    // Serial.println("RRC3 is not ready");
  }
  if(mySerial.available()){
    Serial.println("Received packet");
  unsigned long startTime = millis();
  Serial.print("Received: ");
  String result;
  unsigned long milliseconds = 5000;
  while (millis() - startTime < milliseconds)
  {
    if (mySerial.available())
    {
      char c = mySerial.read();
      Serial.write(c);
      result += c; // append to the result string
    }
  }
  Serial.println(); // new line after timeout.
  }   
}
