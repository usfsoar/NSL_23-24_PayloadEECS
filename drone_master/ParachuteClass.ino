class EmergencyTrigger
{
  private:
    float _acc_critical = 0;
    float _height_critical = 0;

public:
  int state = 0;
  
  EmergencyTrigger(float crit_acc, float crit_height){
    _acc_critical = crit_acc;
    _height_critical = crit_height;
  }
  void checkState(float acceleration, float height, bool low_battery, bool lora_response);
  void jettisonTrigger();
  void abortTrigger();
};

void EmergencyTrigger::checkState(float acceleration, float height, bool low_battery=false, bool lora_response=false)
{
  switch (state)
  {
    case 0:// standby
      break;
    case 1:// falling
      if((acceleration > _acc_critical && height < _height_critical) || low_battery || lora_response){
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
