#include "emergency_trigger.h"

EmergencyTrigger::EmergencyTrigger(float crit_vel, float crit_height) : _kf_velocity(1.0, 1.0, 1.0), _kf_height(1.0, 1.0, 1.0)
{
    _vel_critical = crit_vel;
    _height_critical = crit_height;
}

bool EmergencyTrigger::checkState(float velocity, float height, bool low_battery, bool lora_response)
{
    float filtered_velocity=_kf_velocity.update(velocity);
    float filtered_height=_kf_height.update(height);
    if((_kf_height.checkOutlier(height) || height > 2000) || (_kf_velocity.checkOutlier(velocity))) return false;
    velocity = filtered_velocity;
    height = filtered_height;
    switch (state)
    {
    case 0: // standby
        break;
    case 1: // falling
        if ((velocity > _vel_critical && height < _height_critical) || low_battery || lora_response)
        {
            abortTrigger();
        }
        break;
    case 2: // abort
        break;
    }
    return true;
}

void EmergencyTrigger::jettisonTrigger()
{
    state = 1;
}

void EmergencyTrigger::abortTrigger()
{
    state = 2;
}
