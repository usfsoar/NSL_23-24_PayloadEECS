class DCMotor {
 private:
    const int pwmPin;
    const int highDelay;
    const int lowDelay;
    bool iter;

 public:
    DCMotor(int pwmPin, int highDelay, int lowDelay) : pwmPin(pwmPin), highDelay(highDelay), lowDelay(lowDelay), iter(false) {}

    void DC_SETUP() {
      Serial.begin(9600); 
      pinMode(pwmPin, OUTPUT); 
    }

    void DC_MOVE() {
      int val = 255;
      int del = highDelay;
      if(iter) val = 0; 
      if(iter) del = lowDelay; 
      analogWrite(pwmPin, val); 
      iter = !iter; 
      delay(del);
    }

    void DC_STOP() {
      analogWrite(pwmPin, 0);
    }
};
