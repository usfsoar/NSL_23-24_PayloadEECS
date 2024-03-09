class EmergencyTrigger
{
  private:
    float _vel_critical = 0;
    float _height_critical = 0;

public:
  int state = 0;
  
  EmergencyTrigger(float crit_vel, float crit_height){
    _vel_critical = crit_vel;
    _height_critical = crit_height;
  }
  void checkState(float velocity, float height, bool low_battery, bool lora_response);
  void jettisonTrigger();
  void abortTrigger();
};

void EmergencyTrigger::checkState(float velocity, float height, bool low_battery=false, bool lora_response=false)
{
  switch (state)
  {
    case 0:// standby
      break;
    case 1:// falling
      if((velocity > _vel_critical && height < _height_critical) || low_battery || lora_response){
        abortTrigger();
      }
      break;
    case 2:// abort
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
