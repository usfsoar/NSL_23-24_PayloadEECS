#include "EmergencyTrigger.h"

EmergencyTrigger::EmergencyTrigger(float crit_vel, float crit_height)
{
    _vel_critical = crit_vel;
    _height_critical = crit_height;
}

void EmergencyTrigger::checkState(float velocity, float height, bool low_battery, bool lora_response)
{
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
}

void EmergencyTrigger::jettisonTrigger()
{
    state = 1;
}

void EmergencyTrigger::abortTrigger()
{
    state = 2;
}
