
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"

#define stepPin 10
#define dirPin 11
#define buzzerPin 1

#define SEALEVELPRESSURE_HPA (1013.25)

static const int microDelay = 900;
static const int betweenDelay = 250;

Adafruit_BMP3XX bmp;
void bmp_setup() {
  Wire.begin(13, 12);
  while (!Serial);
  Serial.println("Adafruit BMP388 / BMP390 test");
  if (!bmp.begin_I2C()) {   // hardware I2C mode, can pass in address & alt Wire
    //if (! bmp.begin_SPI(BMP_CS)) {  // hardware SPI mode  
    //if (! bmp.begin_SPI(BMP_CS, BMP_SCK, BMP_MISO, BMP_MOSI)) {  // software SPI mode
      Serial.println("Could not find a valid BMP3 sensor, check wiring!");
      Serial.println("Sensor setup error");
      return;
    }

    // Set up oversampling and filter initialization
    bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
    bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
    bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
    bmp.setOutputDataRate(BMP3_ODR_50_HZ);
}
float GetAltitude(){
  if (!bmp.performReading()) {
    Serial.println("Failed to perform reading :(");
    // Attempt to reconnect to the sensor
    bmp_setup();
    delay(100);
    return 0;
  }
  return bmp.readAltitude(SEALEVELPRESSURE_HPA);
}


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
class BuzzerNotify{
public:
  BuzzerNotify(int pin):pin_number(pin){};

  void Setup(){
    pinMode(pin_number, OUTPUT);
  };
  void Check(){
    curr_cycles++;
    if(curr_cycles > MAX_CYCLES && !beeping){
      digitalWrite(pin_number, HIGH);
      beeping = true;
    }
    if(curr_cycles > MAX_CYCLES_ON){
      digitalWrite(pin_number,LOW);
      beeping = false;
      Reset();
    }
  };
  void Trigger(){
    if(!beeping){
      digitalWrite(pin_number, HIGH);
      delay(50);
      digitalWrite(pin_number, LOW);
    }
  };
  void Reset(){
    curr_cycles =0;
  };
private:
  int pin_number;
  bool beeping=false;
  const uint32_t MAX_CYCLES = 18000000;
  const uint32_t MAX_CYCLES_ON = 19500000;
  uint32_t curr_cycles =0;
};
BuzzerNotify buzzerNotify=BuzzerNotify(buzzerPin);

class Deployment{
public:
  Deployment(){};
  void TriggerProcedure(){
    if(!_active){
      _active = true;
      _last_checkpoint = millis();
    }
  };
  void Stop(){
    _active = false;
  };
  void ProcedureCheck(){
    if(!_active) return;
    const uint32_t curr_duration = millis()-_last_checkpoint;
    if(!_forward &&  curr_duration < _move_duration){
      Serial.println("Deploying forward...");
      moveStepper(-90,0.9);
    }
    else if(!_forward && curr_duration >= _move_duration){
      Serial.println("Stopping forward deploy");
      _forward = true;
      _last_checkpoint = millis();
    }
    else if(_forward && !_nimble && curr_duration < _nimble_duration){
      Serial.println("Allowing time to deploy...");
    }
    else if(_forward && !_nimble && curr_duration >= _nimble_duration){
      Serial.println("Triggering retract");
      _nimble = true;
      _last_checkpoint = millis();
    }
    else if(_forward && _nimble && !_retract && curr_duration < _move_duration){
      Serial.println("Retracting back");
      moveStepper(90,0.8);
    }
    else if(_forward && _nimble && !_retract && curr_duration > _reset_duration){
      Serial.println("Retracting completed");
      _retract = true;
    }
    if(_forward && _nimble && _retract){
      _active = false;
    }
  };
  void Reset(){
    _forward = false;
    _nimble = false;
    _retract = false;
    _last_checkpoint = 0;
    _active = false;
  };
  void Retract(){
    _forward = true;
    _nimble=true;
    _retract = false;
     _last_checkpoint = millis();
    _active = true;
  }

private:
  bool _started = false;
  bool _active = false;
  bool _forward = false;
  bool _nimble = false;
  bool _retract = false;
  uint32_t  _last_checkpoint  =0;
  uint32_t _move_duration = 43000; //43 seconds
  uint32_t _reset_duration = 25000; //Around half of move duration
  uint32_t _nimble_duration = 10000; //10 seconds
};
Deployment deployment;

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
              Serial.println("Stoping deployment\n");
              deployment.Stop();
            }
            else if(value_str == "RESET"){
              pCharacteristic->setValue("OK");
              pCharacteristic->notify();
              Serial.println("Resetting deployment state\n");
              deployment.Reset();
            }
            else if(value_str =="RETRACT"){
              pCharacteristic->setValue("OK");
              pCharacteristic->notify();
              Serial.println("Rtracting deployment\n");
              deployment.Retract();
            }
            else if(value_str =="BEEP"){
              pCharacteristic->setValue("OK");
              pCharacteristic->notify();
              Serial.println("Rtracting deployment\n");
              buzzerNotify.Trigger();
            }
        }
    }
};


void setup() {
  delay(2000);
  Serial.begin(115200);
  buzzerNotify.Setup();
  //Stepper setup------------------
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  //BMP setup---------------------
  bmp_setup();
  
  //Bluetooth setup---------------
  BLEDevice::init("SOAR_DeployModule");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  BLEService *pService = pServer->createService("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");
  pCharacteristic = pService->createCharacteristic(
                      "6E400002-B5A3-F393-E0A9-E50E24DCCA9E",
                      BLECharacteristic::PROPERTY_NOTIFY
                    );

  pCharacteristic->addDescriptor(new BLE2902());
  BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(
          "6E400003-B5A3-F393-E0A9-E50E24DCCA9E",
          BLECharacteristic::PROPERTY_WRITE
        );
  pRxCharacteristic->setCallbacks(new MyCallbacks());
  pService->start();
  // Start advertising
  pServer->getAdvertising()->start();
  buzzerNotify.Trigger();
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
  // Serial.print("Altitude:");
  // Serial.println(GetAltitude());
  buzzerNotify.Check();
}


/*

void loop(){
    moveStepper(90, 0.95);
    delay(1000);
}


*/
