#ifndef EMERGENCYTRIGGER_H
#define EMERGENCYTRIGGER_H
#include "simple_kalman_filter.h"

class EmergencyTrigger
{
private:
    float _vel_critical = 0;
    float _height_critical = 0;
    KalmanFilter _kf_velocity;
    KalmanFilter _kf_height;
public:
    int state = 0;

    EmergencyTrigger(float crit_vel, float crit_height);
    bool checkState(float velocity, float height, bool low_battery = false, bool lora_response = false);
    void jettisonTrigger();
    void abortTrigger();
};

#endif // EMERGENCYTRIGGER_H
