
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "buzzer_notify.h"
#include "MyVL53L0X.h"

//TODO: Get rid of whatever this library is doing
#include <HardwareSerial.h>
#include "soar_barometer.h"
#include "DCMotor.h"
#include "ota_update.h"
#include "SOAR_Lora.h"

#define DEBUG_ALT false
#define DEBUG_BUZZ false
#define DEBUG_TRSHSET false
#define TEST_MOTOR false
#define TEST_MOTOR_BACK false

#define stepPin A3
#define dirPin A2
#define motorInterfaceType 1 //TODO: Get rid of this
#define buzzerPin A0

//ALTIMETER VARIABLES
#define SEALEVELPRESSURE_HPA (1013.25)
float altimeter_latest;
int ALT_TRSH_CHECK=850; // Use -10 for parking lot test and maybe change it on location
int LOW_ALT_TRSH_CHECK=300;

OTA_Update otaUpdater("soar-deploy", "TP-Link_BCBD", "10673881");

SOAR_Lora lora("5", "5", "433000000"); // LoRa

//STEPPER MOTOR DELAYS
static const int microDelay = 900;
static const int betweenDelay = 250;

//DC motor
DCMotor motor(A2, 50, 50);

SOAR_BAROMETER barometer;

float previous_altitude = -300;
float max_candidate = -300;
int alt_trigger_count = 0;
int low_alt_trigger_count = 0;
float immediate_previous = -6000;
int altitudeTrigger(float current_altitude)
{
#if DEBUG_ALT
  Serial.print("Dif:");
  Serial.println(current_altitude - previous_altitude);
  Serial.print("Prev:");
  Serial.println(previous_altitude);
#endif
  int res = 0;
  // Check if the altitude is decreasing and above ALT_TRSH_CHECK
  if ((current_altitude > ALT_TRSH_CHECK) && (current_altitude - previous_altitude < -2))
  {
    res = 1;
  }
  if (current_altitude > previous_altitude)
    if (current_altitude - immediate_previous < 800 || immediate_previous == -60000)
    { // Default value for errors
      previous_altitude = current_altitude;
    }
  if (current_altitude - immediate_previous > 800)//If altitude shows sudden changes it must be a glitch
    res = 0;
  // Update previous_altitude for the next function call
  immediate_previous = current_altitude;
  return res;
}



BuzzerNotify buzzerNotify = BuzzerNotify(buzzerPin);

MyVL53L0X distanceSensor;

class Deployment
{
private:
  int _state = 0; // _standby = 0; _forward = 1; _wait = 2; _retract = 3; _complete = 4;  _paused = 5;
  const char *message[6]={"STANDBY", "FORWARD", "WAITING", "RETRACTING", "COMPLETED", "PAUSED"};
  bool sensor_trigger = false;
  uint32_t _forward_checkpoint = 0;
  uint32_t _wait_checkpoint = 0;
  uint32_t _retract_checkpoint = 0;
  uint32_t _last_checkpoint = 0;
  uint32_t _forward_duration = 3500;   // 43 seconds
  uint32_t _retract_duration = 8000;  // Around half of move duration
  uint32_t _wait_duration = 60000; // 10 seconds
  int last_state = 0;
  bool _warn = false;
  int  fwd_sensor_checks = 0;
  int retract_sensor_checks = 0;
public:
  Deployment(){};

  void Deploy()
  {
    if (_state==0)
    {
      _state = 1;
    }
    if(_state==5){
      _state=last_state;
      switch (last_state){
        case 1:
          _forward_checkpoint=millis();
          break;
        case 2:
          _wait_checkpoint=millis();
          break;
        case 3:
          _retract_checkpoint=millis();
          break;
      }
    }
  };

  void Stop()
  {
    last_state=_state;
    _state = 5;
    motor.DC_STOP();
  };

  void ProcedureCheck()
  {
    uint16_t distance;
    switch (_state){
      case 0://standby
        break;
      case 1://forward
        if(_forward_checkpoint==0){
          GetStatus();
          for(int i=0; i<5; i++){
            buzzerNotify.Trigger();
            delay(100);
          }
          _forward_checkpoint=millis();
        }
        //Sensor and time logic comes first
        distance = distanceSensor.readDistance();
        Serial.println(distance);
        sensor_trigger = distance>500;
        if(sensor_trigger) fwd_sensor_checks++;
        else fwd_sensor_checks = 0;
        if(fwd_sensor_checks>3 || (millis()-_forward_checkpoint)>_forward_duration){
          if(sensor_trigger){
            Serial.println("Stop triggered by sensor");
          }
          Serial.println("Stopped.");
          motor.DC_STOP();
          _state=2;
        }
        else{
          // Move forward logic comes second
          motor.DC_MOVE(50);
        }
        break;
      case 2://wait
        if(_wait_checkpoint==0){
          GetStatus();
          _wait_checkpoint=millis();
        }
        motor.DC_STOP();
        if((millis()-_wait_checkpoint)>_wait_duration){
          _state=3;
        }
        break;
      case 3://retract
        if(_retract_checkpoint==0){
          GetStatus();
          for(int i=0; i<5; i++){
            buzzerNotify.Trigger();
            delay(100);
          }
          _retract_checkpoint=millis();
        }
        //Sensor and time logic comes first
        distance = distanceSensor.readDistance();
        Serial.println(distance);
        sensor_trigger = distance<90;
        if(sensor_trigger) retract_sensor_checks++;
        else retract_sensor_checks = 0;
        if(retract_sensor_checks>3 || (millis()-_retract_checkpoint)>_retract_duration){
          if(sensor_trigger){
            Serial.println("Stop triggered by sensor");
          }
          Serial.println("Stopped.");
          motor.DC_STOP();
          _state=4;
        }
        else{
          // Move back logic comes second
          motor.DC_MOVE(-50);
        }
        break;
      case 4://complete
        motor.DC_STOP();
        break;
      case 5://paused
        motor.DC_STOP();
        break;
    }
  };
  void Reset()
  {
    _state = 0;
    _forward_checkpoint = 0;
    _wait_checkpoint = 0;
    _retract_checkpoint = 0;
    _warn=false;
  };
  void Retract()
  {
    if(_state==0 || _state==2) _state=3;
  }
  String GetStatus()
  {
    return message[_state];
  };
};
Deployment deployment;

BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;

class MyServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer)
  {
    deviceConnected = true;
  };

  void onDisconnect(BLEServer *pServer)
  {
    deviceConnected = false;
  }
};

class MyCallbacks : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    std::string value = pCharacteristic->getValue();
    if (value.length() > 0)
    {
      String value_str = "";
      for (int i = 0; i < value.length(); i++)
        value_str += value[i];
      Serial.print("Received Value: ");
      Serial.println(value_str);
      if (value_str == "DEPLOY")
      {
        pCharacteristic->setValue("OK");
        pCharacteristic->notify();
        Serial.println("Deploy procedure\n");
        deployment.Deploy();
      }
      else if (value_str == "STOP")
      {
        pCharacteristic->setValue("OK");
        pCharacteristic->notify();
        Serial.println("Stoping deployment\n");
        deployment.Stop();
      }
      else if (value_str == "RESET")
      {
        pCharacteristic->setValue("OK");
        pCharacteristic->notify();
        Serial.println("Resetting deployment state\n");
        deployment.Reset();
      }
      else if (value_str == "RETRACT")
      {
        pCharacteristic->setValue("OK");
        pCharacteristic->notify();
        Serial.println("Rtracting deployment\n");
        deployment.Retract();
      }
      else if (value_str == "BEEP")
      {
        pCharacteristic->setValue("OK");
        pCharacteristic->notify();
        Serial.println("Rtracting deployment\n");
        buzzerNotify.Trigger();
      } else if(value_str == "STATUS") {
          String  sts  = deployment.GetStatus();
          String stat = "DEPLOY-STATUS:"+ sts;
          std::string stat_std = stat.c_str(); // Convert Arduino String to std::string
          pCharacteristic->setValue(stat_std); // Set the value using std::string
      }
    }
  }
};



void setup()
{
  // Set the maximum speed and acceleration

#if TEST_MOTOR
  deployment.TriggerProcedure();
#if TEST_MOTOR_BACK
deployment.Retract();
#endif
#endif
  Serial.begin(115200);
  Wire.begin();
  //LORA SETUP
  lora.begin();

  buzzerNotify.Setup();
  // Stepper setup------------------
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  // BMP setup---------------------
  buzzerNotify.Trigger();

  // Bluetooth setup---------------
  BLEDevice::init("SOAR_DeployModule");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  BLEService *pService = pServer->createService("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");
  pCharacteristic = pService->createCharacteristic(
      "6E400002-B5A3-F393-E0A9-E50E24DCCA9E",
      BLECharacteristic::PROPERTY_NOTIFY);

  pCharacteristic->addDescriptor(new BLE2902());
  BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(
      "6E400003-B5A3-F393-E0A9-E50E24DCCA9E",
      BLECharacteristic::PROPERTY_WRITE);
  pRxCharacteristic->setCallbacks(new MyCallbacks());
  pService->start();
  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
  buzzerNotify.Trigger();
  barometer.Initialize();
  buzzerNotify.Trigger();
  motor.DC_SETUP();
  buzzerNotify.Trigger();

  otaUpdater.Setup();

  //Distance sensor setup
  
  distanceSensor.begin();
  delay(500);
  lora.sendCommand("AWAKE");
}

void loop()
{
  // Disconnecting
  if (!deviceConnected && oldDeviceConnected)
  {
    delay(500);                  // Give the Bluetooth stack the chance to get things ready
    pServer->startAdvertising(); // Restart advertising
    Serial.println("Advertising started");
    oldDeviceConnected = deviceConnected;
  }

  // // Connecting
  if (deviceConnected && !oldDeviceConnected)
  {
    // do stuff here on connecting
    oldDeviceConnected = deviceConnected;
  }

  // Automated Altitude Trigger Check
  float altitude = barometer.get_last_altitude_reading();
  altimeter_latest = altitude;
#if DEBUG_ALT
  Serial.print("Altitude: ");
  Serial.println(altitude);
#endif

  int descending = altitudeTrigger(altitude);
  if (descending==1)
  {
    if (alt_trigger_count > 5)
    { // Must make sure that the trigger is not a false positive
      Serial.println("Triggering deployment");
      deployment.Deploy();
    }
    else
    {
      alt_trigger_count++; // This must be protected under else to prevent overflow
    }
  }
  else if(descending==2){
    if(low_alt_trigger_count>5){
      Serial.println("Low altitude detected");
      if(deployment.GetStatus()=="FORWARD"){ //In case we haven't finished extended by the time we reach the lower altitude
        deployment.Stop();
        deployment.Reset();
      }
      deployment.Retract();
    } else {
      low_alt_trigger_count++;
    }
  }
  

  // Deployment Procedure Constant Check
  deployment.ProcedureCheck();

  if (lora.available())
  {
    String incomingString = "";
    Serial.print("Request Received: ");
    String data_str = lora.read();
    if (data_str == "PING")
    {
      lora.queueCommand("PONG");
    }
    else if (data_str == "DEPLOY")
    {
      Serial.println("Deployment Triggered");
      deployment.Deploy();
      lora.queueCommand("DEPLOY:TRIGGERING");
    }
    else if (data_str == "STOP")
    {
      deployment.Stop();
      lora.queueCommand("DEPLOY:STOPING");
    }
    else if (data_str == "RESET")
    {
      deployment.Reset();
      lora.queueCommand("DEPLOY:RESETING");
    }
    else if (data_str == "STATUS")
    {
      String stat = "DEPLOY-STATUS:" + deployment.GetStatus();
      lora.queueCommand(stat);
    }
    else if (data_str == "RETRACT")
    {
      deployment.Retract();
      lora.queueCommand("DEPLOY:RETRACTING");
    }
    else if (data_str == "ALTITUDE")
    {
      char altimeter_latest_str[9];
      dtostrf(altimeter_latest, 4, 2, altimeter_latest_str);
      char altitude_str[100] = "ALTITUDE:";
      strcat(altitude_str, altimeter_latest_str);
      lora.queueCommand(altitude_str);
    }
    else if (data_str == "DISTANCE")
    {
      char distance_data[5];
      sprintf(distance_data, "%u", distanceSensor.readDistance());
      char distance_str[100] = "DISTANCE:";
      strcat(distance_str, distance_data);
      lora.queueCommand(distance_str);
    }
    else if (data_str.indexOf("THRESHOLD") >= 0)
    {
      try
      {
        for (int i = 0; i < data_str.length(); i++)
        {
          if (data_str[i] == ':')
          {
            String curstr = data_str.substring(i + 1);
            int new_trsh = curstr.toInt();
            ALT_TRSH_CHECK = new_trsh;
            break;
          }
        }
#if DEBUG_TRSHSET
        Serial.print("New Trsh: ");
        Serial.println(ALT_TRSH_CHECK);
#endif
        lora.queueCommand("THRESHOLD:SET");
      }
      catch (String error)
      {
        lora.queueCommand("THRESHOLD:ERROR");
      }
    }
    else
    {
      lora.queueCommand("INVALID");
    }
  }
  // Vital Sign Indicator
  lora.handleQueue();
  buzzerNotify.Check();
  otaUpdater.Handle();
}
