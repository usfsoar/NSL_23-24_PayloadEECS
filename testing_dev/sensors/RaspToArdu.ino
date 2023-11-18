#include <Wire.h>

#define SLAVE_ADDRESS 0x08
byte data_to_send = 0;
byte data_to_echo = 0;
char output[]="This is a test string\n";

void setup() {
    Serial.begin(9600);
    Wire.begin(SLAVE_ADDRESS);
    Wire.onRequest(sendData);
    Wire.onReceive(receiveData);
}

void loop() {
    data_to_send = random(0, 255);
    //Serial.println(data_to_send); // change this to the data you want to send
    delay(1000);
}

void sendData() {
    Wire.write(output);
}

void receiveData()
{
  int i=0;
   while (Wire.available() > 0) {
    char receivedChar = Wire.read();
    i++;
    if(i<=1) continue;
    
    Serial.print(receivedChar);
    
  }
  Serial.println();
}