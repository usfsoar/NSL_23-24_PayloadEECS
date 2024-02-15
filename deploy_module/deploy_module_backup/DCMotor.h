#include <ESP32Servo.h>

class DCMotor {
  private:
    const int pwmPin;
    int highDelay;
    int lowDelay;
    bool iter;
    Servo esc; // Create a servo object
    bool started=false;

 public:
    DCMotor(int pwmPin, int highDelay, int lowDelay) : pwmPin(pwmPin), highDelay(highDelay), lowDelay(lowDelay), iter(false) {}

    void DC_SETUP();

    void DC_MOVE(int speed);

    void DC_STOP();
};

