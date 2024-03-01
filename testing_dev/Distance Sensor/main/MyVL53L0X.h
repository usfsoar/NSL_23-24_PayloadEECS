#ifndef MYVL53L0X_H
#define MYVL53L0X_H

#include <Arduino.h>
#include <Wire.h>
#include <VL53L0X.h>

class MyVL53L0X {
  private:
    VL53L0X sensor;

  public:
    void begin();
    uint16_t readDistance();
};

#endif
