class DCMotor {
  private:
    const int pwmPin;
    int highDelay;
    int lowDelay;
    bool iter;

 public:
    DCMotor(int pwmPin, int highDelay, int lowDelay) : pwmPin(pwmPin), highDelay(highDelay), lowDelay(lowDelay), iter(false) {}

    void DC_SETUP();

    void DC_MOVE(int speed, uint32_t time);

    void DC_STOP();
};

