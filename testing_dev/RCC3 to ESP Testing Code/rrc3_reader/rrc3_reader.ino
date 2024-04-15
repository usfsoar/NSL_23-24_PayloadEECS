// #include <SoftwareSerial.h>
#include <HardwareSerial.h>

// SoftwareSerial mySerial(7, 6));
HardwareSerial mySerial(0);
void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(9600);
  digitalWrite(LED_BUILTIN, HIGH);
  mySerial.begin(9600, SERIAL_8N1, -1, -1);
  // mySerial.begin(9600);
  mySerial.begin(9600);
  digitalWrite(LED_BUILTIN, LOW);
  // pinMode(10, OUTPUT);
  Serial.println("Setup complete");
}





void loop() {
  // put your main code here, to run repeatedly:
  // mySerial.listen();
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
      if(c == '\r')
            break;
        result += c;
      Serial.write(c);
      result += c; // append to the result string
    }
  }
  Serial.println(); // new line after timeout.
  }   
}
