#ifndef SOAR_IMU_h
#define SOAR_IMU_h
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>


class SOAR_IMU {
  public:
    SOAR_IMU(); // Constructor
    // Declare methods and variables here
    void BNO_SETUP();
    float* GET_ACCELERATION();
    float* GET_LINEARACCEL();
    float* GET_GRAVITY();
    float* GET_GYROSCOPE();
    float* GET_QUAT();
    uint32_t* GET_VELOCITY();

  private:
    // Declare private variables and methods here
    Adafruit_BNO055 bno;
    int fail_count;
    // previous time value
    uint32_t prev_time;
    //previous iteration acceleration values for calculating velocity
    uint32_t prev_accel_x;
    uint32_t prev_accel_y;
    uint32_t prev_accel_z;
    //count iterations of main loop:

  public:
    //declare public variables here:
    uint32_t loop_iterations;
};

#endif
