// Pin for PWM output
const int pwmPin = 9;
const int highDelay = 50;
const int lowDelay = 50;

void setup() {
  // Set the PWM pin as an output
  Serial.begin(9600);
  pinMode(A0, OUTPUT);
}

bool iter = false;

void loop() {
  // Output the PWM signal
  int val = 255;
  int del = highDelay;
  if(iter) val = 0;
  if(iter) del = lowDelay;
  analogWrite(A0, val);
  iter = !iter;
  
  // Delay for demonstration purposes
  delay(del); // Adjust as needed
}
