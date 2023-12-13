
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define stepPin 10
#define dirPin 11

static const int microDelay = 900;
static const int betweenDelay = 250;

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;

// Function to move the motor a certain number of degrees
void moveStepper(int degrees, double vel){
    if(degrees == 0) return;
    //If degrees negative dir=0 else dir=1
    bool dir = degrees > 0 ? 1 : 0;
    double steps = double(degrees/360.0)*1725.0; // 200 steps per rotation  
    if(steps==0) return;
    if(steps<0) steps = steps * -1;
    if(dir) digitalWrite(dirPin,HIGH); // Enables the motor to move in a particular direction
    else digitalWrite(dirPin, LOW);
    double micro_scaled = microDelay + (1-vel) * 10000;
  // Makes 200 pulses for making one full cycle rotation
    for(double x = 0; x < steps; x++) {
        digitalWrite(stepPin, HIGH); 
        delayMicroseconds(micro_scaled); 
        digitalWrite(stepPin, LOW); 
        delayMicroseconds(betweenDelay); 
    }
}

class Deployment{
public:
  Deployment()
  void TriggerProcedure(){
    if(!_active){
      _active = true;
      _last_checkpoint = millis();
    }
  }
  void StopProcedure(){
    _active = false;
  }
  void ProcedureCheck(){
    if(!_active) return;
    const uint32_t curr_duration = millis()-_last_checkpoint;
    if(!_forward &&  curr_duration < _move_duration){
      Serial.println("Deploying forward...");
      moveStepper(90,0.95);
    }
    else if(!_forward && _current_duration >= _move_duration){
      Serial.print("Stopping forward deploy");
      _forward = true;
      _last_checkpoint = millis();
    }
    else if(_forward && !_nimble && curr_duration < _nimble_duration){
      Serial.print("Allowing time to deploy...");
    }
    else if(_forward && !_nimble && curr_duaration >= _nimble_duration){
      Serial.print("Triggering retract");
      _nimble = true;
      _last_checkpoint = millis();
    }
    else if(_forward && _nimble && !_retract && curr_duration < _move_duration){
      Serial.print("Retracting back");
      moveStepper(-90,0.95);
    }
    else if(_forward && _nimble && !_retract && curr_duration > _move_duration){
      Serial.print("Retracting completed";)
      _retract = true;
    }
    if(_forward && _nimble && _retract){
      _active = false;
    }
  }
  void Reset(){
    _active = false;
    _forward = false;
    _nimble = false;
    _retract = false;
    _last_checkpoint = 0; 
  }

private:
  bool _started = false;
  bool _active = false;
  bool _forward = false;
  bool _nimble = false;
  bool _retract = false;
  uint32_t  _last_checkpoint  =0;
  uint32_t _move_duration = 20000; //20 seconds
  uint32_t _nimble_duration = 5000; //5 seconds
  uint32_t _current_duration = 0;
};
Deployment deployment();

class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

class MyCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        std::string value = pCharacteristic->getValue();
        if (value.length() > 0) {
            String value_str = "";
            for (int i = 0; i < value.length(); i++)
                value_str += value[i];
            Serial.print("Received Value: ");
            Serial.println(value_str);
            if(value_str=="DEPLOY"){
              pCharacteristic->setValue("OK");
              pCharacteristic->notify();
              Serial.println("Deploy procedure\n");
              deployment.TriggerProcedure();
            }
            else if(value_str == "STOP"){
              pCharacteristic->setValue("OK");
              pCharacteristic->notify();
              Serial.println("Stoping deployment state\n");
              deployment.Stop();
            }
            else if(value_str == "RESET"){
              pCharacteristic->setValue("OK");
              pCharacteristic->notify();
              Serial.println("Resetting deployment state\n");
              deployment.Reset();
            }
        }
    }
};

void setup() {
  Serial.begin(115200);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  // Create the BLE Device
  BLEDevice::init("SOAR_DeployModule");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                      "6E400002-B5A3-F393-E0A9-E50E24DCCA9E",
                      BLECharacteristic::PROPERTY_NOTIFY
                    );

  pCharacteristic->addDescriptor(new BLE2902());

  // Create another BLE Characteristic for receiving messages
  BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(
          "6E400003-B5A3-F393-E0A9-E50E24DCCA9E",
          BLECharacteristic::PROPERTY_WRITE
        );

  pRxCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
}

void loop() {

  // Disconnecting
  if (!deviceConnected && oldDeviceConnected) {
    delay(500); // Give the Bluetooth stack the chance to get things ready
    pServer->startAdvertising(); // Restart advertising
    Serial.println("Advertising started");
    oldDeviceConnected = deviceConnected;
  }

  // Connecting
  if (deviceConnected && !oldDeviceConnected) {
    // do stuff here on connecting
    oldDeviceConnected = deviceConnected;
  }
  deployment.ProcedureCheck();
}


/*

void loop(){
    moveStepper(90, 0.95);
    delay(1000);
}


*/