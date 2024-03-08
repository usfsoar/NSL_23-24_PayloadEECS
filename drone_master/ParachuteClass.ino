class EmergencyTrigger {

  
public:
  int state = 0;

  void checkState();
  void jettisonTrigger();
  void abortTrigger();
};

EmergencyTrigger::checkState(int state)
{
  switch (state) {
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


EmergencyTrigger::jettisonTrigger()
{
 state = 1 
 checkState(state);
}

EmergencyTrigger::abortTrigger()
{
  state = 2
  checkState(state);
}

