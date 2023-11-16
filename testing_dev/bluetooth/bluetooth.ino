#include<SoftwareSerial.h>

/* Create object named bt of the class SoftwareSerial */ 
SoftwareSerial bt(2,3); /* (Rx,Tx) */ 

void setup(){
  bt.begin(9600); /* Define baud rate for software serial communication */
  Serial.begin(9600);  /* Define baud rate for serial communication */
  sendATcommand("AT", 500);
  sendATcommand("AT+NAME?", 500);
}

void loop(){
    if (bt.available()) /* If data is available on serial port */
    {
    	Serial.write(bt.read());      /* Print character received on to the serial monitor */
    }
}


String sendATcommand(const char *toSend, unsigned long milliseconds) {
  String result;
  Serial.print("Sending: ");
  Serial.println(toSend);
  bt.println(toSend);
  unsigned long startTime = millis();
  Serial.print("Received: ");
  while (millis() - startTime < milliseconds) {
    if (bt.available()) {
      char c = bt.read();
      Serial.write(c);
      result += c;  // append to the result string
    }
  }
  Serial.println();  // new line after timeout.
  return result;
}