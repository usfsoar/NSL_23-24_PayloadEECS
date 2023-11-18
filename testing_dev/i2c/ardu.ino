#include <Wire.h>

static const char messageInterrupt = A1;
static const char numberInterrupt = 5;

int flag = 0;

void setup() {
    pinMode(numberInterrupt, OUTPUT);
    pinMode(messageInterrupt, OUTPUT);

    Serial.begin(9600);

    Wire.begin(0x12);
    Wire.onReceive(receiveData);
    Wire.onRequest(sendData);
    digitalWrite(numberInterrupt, LOW);
    digitalWrite(messageInterrupt, LOW);

    trigger_numberInterrupt();
    delay(100);
    trigger_messageInterrupt();
}

void loop() {}

void trigger_numberInterrupt() {
    digitalWrite(numberInterrupt, HIGH);
    flag = 2;
    delay(10);
    digitalWrite(numberInterrupt, LOW);
}

void trigger_messageInterrupt() {
    digitalWrite(messageInterrupt, HIGH);
    flag = 1;
    delay(10);
    digitalWrite(messageInterrupt, LOW);
}

void receiveData(int bytes) {}

void sendData() {
    if(flag == 2) {
      Wire.write("Hello foo");
      delay(1000);
    }

    if(flag == 1) {
        Wire.write("Hello bar");
        delay(1000);  //if no sync : add more delay.
    }

    flag = 0;
}