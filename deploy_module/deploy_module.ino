#include <Arduino.h>
#include<SoftwareSerial.h>

// Define pin connections & motor's steps per revolution
#define stepPin 5
#define dirPin 4
#define potPin A5

//static const int microDelay = 5000;
//static const int betweenDelay = 250;
static const int microDelay = 900;
static const int betweenDelay = 250;

/* Create object named bt of the class SoftwareSerial */ 
SoftwareSerial bt(2,3); /* (Rx,Tx) */ 

// Setup the pins on startup
void setup() {
  Serial.begin(9600);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  bt.begin(9600); /* Define baud rate for software serial communication */
}

bool turnMotor=false;

void loop(){
}

// Function to move the motor a certain number of degrees
void moveStepper(int degrees, double vel)
//~HinazukiKayo: if a negative value is passed then we would have to change the other function that gives us the final angle to move, to also give back a bool instead of a negative value. Maybe it would be better to just handle it in this function as in if degrees<0 then dir = 1 idfk?
{
    if(degrees == 0) return;
    //If degrees negative dir=0 else dir=1
    bool dir = degrees > 0 ? 1 : 0;
    double steps = double(degrees/360.0)*200.0; // 200 steps per rotation  
    if(steps==0) return;
    if(steps<0) steps = steps * -1;
    if(dir) digitalWrite(dirPin,HIGH); // Enables the motor to move in a particular direction
    else digitalWrite(dirPin, LOW);
    double micro_scaled = microDelay + (1-vel) * 10000;
  // Makes 200 pulses for making one full cycle rotation
    for(double x = 0; x < steps; x++) {
        digitalWrite(stepPin, HIGH); 
        delayMicroseconds(micro_scaled); 
        digitalWrite(stepPin, LOW); 
        delayMicroseconds(betweenDelay); 
    }
}

//for bluetooth commands
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

String receiveCommand(){
  String result;
  if (bt.available()) {
    char c = bt.read();
    Serial.write(c);
    result += c;  // append to the result string
  }
  Serial.println(result);
  return result;
}
