class EmergencyTrigger
{
  private:
    float _acc_critical;
    float _height_critical;

public:
  int state = 0;
  
  EmergencyTrigger(float crit_acc, float crit_height){
    _acc_critical = crit_acc;
    _height_critical = crit_height;
  }
  void checkState(float acceleration, float);
  void jettisonTrigger();
  void abortTrigger();
};

void EmergencyTrigger::checkState(float acceleration, float height)
{
  switch (state)
  {
    case 0:
      return;
    case 1:
      //
      break;
    case 2:
      //
      break;
    default:

      break;
  }
}

void EmergencyTrigger::jettisonTrigger()
{
  state = 1;
  checkState(state);
}

void EmergencyTrigger::abortTrigger()
{
  state = 2;
  checkState(state);
}
