#include "_config.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "buzzer_notify.h"
#include "MyVL53L0X.h"
#include "simple_kalman_filter.h"

// TODO: Get rid of whatever this library is doing
#include <HardwareSerial.h>
#include "soar_barometer.h"
#include "DCMotor.h"
#include "ota_update.h"
#include "SOAR_Lora.h"
#include "SOAR_Lora.h"

#define DEBUG_BUZZ false
#define DEBUG_TRSHSET false
#define TEST_MOTOR false
#define TEST_MOTOR_BACK false

#define buzzerPin A0

// ALTIMETER VARIABLES
#define SEALEVELPRESSURE_HPA (1013.25)
float altimeter_latest;

OTA_Update otaUpdater("soar-deploy", "TP-Link_BCBD", "10673881");

SOAR_Lora lora("5", "5", "905000000"); // LoRa

// DC motor
DCMotor motor(A2, 50, 50);

SOAR_BAROMETER barometer;

#if DIGITAL_TWIN
bool GetFakeLoraAvailable(){
  //Send a request for lora message
  byte request = 0x08;
  Serial.write(request);
  Serial.write(request);

  uint32_t timeout_start = millis();
  //Check for a rsponse code 0x11 that will come with a boolean value
  while(millis()-timeout_start<1000){
    if(Serial.available()){
      byte responseCode = Serial.read();
      if(responseCode == 0x09){
        //Read the next byte as a boolean
        if(Serial.available()){
          return Serial.read();
        }
      }
    }
  }
  return false;
}
String GetFakeLora(){
  // Send a request for lora
  Serial.write(0x10);
  Serial.write(0x01);
  uint32_t timeout_start = millis();
  // Wait for a string response that wil come in bytes and ended with a null character
  String response = "";
  while (millis() - timeout_start < 1000)
  {
    if(Serial.available()){
      byte responseCode = Serial.read();
      if (responseCode == 0x11) // Arbitrary response code
      {
        response = "";
        byte res_len = Serial.read();
        // Read the next res_len bytes as a string
        for (int i = 0; i < res_len; i++)
        {
          if (Serial.available())
          {
            response += (char)Serial.read();
          }
        }
        break;
      }
    }
  }
  return response;
}
float GetFakeAltitude()
{
  // Send a request for altitude
  byte request = 0x01; // Arbitrary request code
  Serial.write(request);

  uint32_t timeout_start = millis();
  while (millis() - timeout_start < 1000)
  {
    if (Serial.available())
    {
      byte responseCode = Serial.read();
      if (responseCode == 0x02) // Arbitrary response code
      {
        // Read the next 4 bytes as a float
        union {
          float altitude;
          byte bytes[4];
        } altitudeData;

        for (int i = 0; i < 4; i++)
        {
          if (Serial.available())
          {
            altitudeData.bytes[i] = Serial.read();
          }
          else
          {
            return 44330.00; // Timeout or incomplete data
          }
        }
        return altitudeData.altitude;
      }
    }
  }
  return 44330.00;
}

void SendAltitudeData(float altitude, float maxAltitude, int state, bool outlier)
{
  // Start with a unique identifier byte for this message type
  Serial.write(0x03);
  Serial.write(0x03);

  // Send altitude as 4 bytes
  union {
    float value;
    byte bytes[4];
  } altitudeData;
  altitudeData.value = altitude;
  Serial.write(altitudeData.bytes, sizeof(altitudeData.bytes));

  // Send max altitude as 4 bytes
  union {
    float value;
    byte bytes[4];
  } maxAltitudeData;
  maxAltitudeData.value = maxAltitude;
  Serial.write(maxAltitudeData.bytes, sizeof(maxAltitudeData.bytes));

  // Send state as 4 bytes (int)
  union {
    int value;
    byte bytes[4];
  } stateData;
  stateData.value = state;
  Serial.write(stateData.bytes, sizeof(stateData.bytes));
  //Send outlier a 1 byte (bool)
  
  Serial.write(outlier);
  
}

uint16_t GetFakeDistance(){
  // Send a request for distance
  byte request = 0x05;
  Serial.write(request);
  //Send confirmation byte
  Serial.write(request);

  uint32_t timeout_start = millis();
  while (millis() - timeout_start < 500)
  {
    if (Serial.available())
    {
      byte responseCode = Serial.read();
      if (responseCode == 0x06) // Arbitrary response code
      {
        // Read the next 2 bytes as a uint16_t
        union {
          uint16_t distance;
          byte bytes[2];
        } distanceData;

        for (int i = 0; i < 2; i++)
        {
          if (Serial.available())
          {
            distanceData.bytes[i] = Serial.read();
          }
          else
          {
            return 65535; // Timeout or incomplete data
          }
        }
        return distanceData.distance;
      }
    }
  }
  return 65535;
}
void SendFakeDistanceData(uint16_t distance){
  // Start with a unique identifier byte for this message type
  Serial.write(0x07);
  Serial.write(0x07);
  // Send distance as 2 bytes
  union {
    uint16_t value;
    byte bytes[2];
  } distanceData;
  distanceData.value = distance;
  Serial.write(distanceData.bytes, sizeof(distanceData.bytes));

}
void SendFakeMotor(int dir){
  Serial.println("Requesting fake motor");
  byte request = 0x04;
  Serial.write(request);
  Serial.write(request);
  // Send direction as 4 bytes (int)
  union {
    int value;
    byte bytes[4];
  } dirData;
  dirData.value = dir;
  Serial.write(dirData.bytes, sizeof(dirData.bytes));
}
#endif
class AltitudeTrigger
{
private:
  float _max_height = 0;
  float _h0;
  float _h1;
  float _h2;
  float _prev_altitude = -500;
  float _average = 0;
  float _sum = 0;
  float _max_distance = 0;
  const float _MACH = 175; //0.5 * speed of sound (maximum velocity per second - 171.5)
  uint32_t _last_checkpoint = 0; //for time control
  KalmanFilter _kf;

public:
  int state=0;

  AltitudeTrigger(float H0, float H1, float H2) : _kf(1.0, 1.0, 1.0)
  {
    _h0 = H0;
    _h1 = H1;
    _h2 = H2;
    
  }
  float GetMaxAltitude(){
    return _max_height;
  }
  bool CheckAltitude(float curr_altitude)
  {
    float filtered_alt=_kf.update(curr_altitude);
    if(_kf.checkOutlier(curr_altitude) || curr_altitude > 2000) return false;
    curr_altitude = filtered_alt;
    switch (state)
    {
      case 0://if next value is greater than 100
        if (curr_altitude > _h0){
          state = 1;
         }
        break;
      case 1:
        if ((curr_altitude > _max_height) && (curr_altitude - _prev_altitude > 0))
        {
            _max_height = curr_altitude;
        }
        if ((_max_height-curr_altitude>10) && (curr_altitude - _prev_altitude < 0))
        {
          state = 2;
        }
        break;
      case 2:
        if (curr_altitude < _h2 && curr_altitude > _h1)
        {
          state = 3;
        }
        break;
      case 3:
        if (curr_altitude < _h1)
        {
          state = 4;
        }
        break;
      case 4:
        //retracting
        break;
    }
    _prev_altitude=curr_altitude;
    return true;
  }
  void Reset(){
    state = 0;
    _max_height = 0;
    _kf = KalmanFilter(1.0, 1.0, 1.0);
  } 
  //Get altitude thresholds H0, H1, H2 as float array
  float* GetThresholds(){
    float thresholds[3] = {_h0, _h1, _h2};
    return thresholds;
  }
  //Update altitude thresholds H0, H1, H2
  void UpdateThresholds(float H0, float H1, float H2){
    _h0 = H0;
    _h1 = H1;
    _h2 = H2;
  }

};

AltitudeTrigger altTrigger(914,107,183);

BuzzerNotify buzzerNotify = BuzzerNotify(buzzerPin);

MyVL53L0X distanceSensor;

class Deployment
{
private:
  int _state = 0; // _standby = 0; _forward = 1; _wait = 2; _retract = 3; _complete = 4;  _paused = 5;
  const char *message[6] = {"STANDBY", "FORWARD", "WAITING", "RETRACTING", "COMPLETED", "PAUSED"};
  bool sensor_trigger = false;
  uint32_t _forward_checkpoint = 0;
  uint32_t _wait_checkpoint = 0;
  uint32_t _retract_checkpoint = 0;
  uint32_t _last_checkpoint = 0;
  uint32_t _forward_duration = 5000;   // 2.5 seconds 3500
  uint32_t _retract_duration = 11000;  // Around half of move duration
  uint32_t _wait_duration = 20000; // 10 seconds
  int last_state = 0;
  bool _warn = false;
  int fwd_sensor_checks = 0;
  int retract_sensor_checks = 0;
  KalmanFilter _kf;

public:
  Deployment():_kf(1.0, 1.0, 1.0){};
  void Deploy()
  {
    if (_state == 0)
    {
      _state = 1;
    }
    if (_state == 5)
    {
      _state = last_state;
      switch (last_state)
      {
      case 1:
        _forward_checkpoint = millis();
        break;
      case 2:
        _wait_checkpoint = millis();
        break;
      case 3:
        _retract_checkpoint = millis();
        break;
      }
    }
  };

  void Stop()
  {
    last_state = _state;
    _state = 5;
    motor.DC_STOP();
  };

  void ProcedureCheck()
  {
    uint16_t distance;
    int speed_fwd;
    float filtered_dist;
    bool outlier;
    switch (_state){
      case 0://standby
        break;
      case 1://forward
        if(_forward_checkpoint==0){
          GetStatus();
          for(int i=0; i<5; i++){
            buzzerNotify.Trigger();
            //If kalman filter is not initialized, add the measurement to the buffer
            if(!_kf.initialized){
              _kf.update(distanceSensor.readDistance());
              delay(50);
              _kf.update(distanceSensor.readDistance());
              delay(50);
            }
          }
          
          _forward_checkpoint=millis();
        }
        speed_fwd = 100;
        //Sensor and time logic comes first
        //Check if not DIGITAL_TWIN
        #if !DIGITAL_TWIN
        distance = distanceSensor.readDistance();
        Serial.println(distance);
        #else
        distance = GetFakeDistance();
        SendFakeDistanceData(distance);
        #endif
        //Filter the distance convert distance to float
        filtered_dist = _kf.update(distance);
        //Check for outlier
        outlier = _kf.checkOutlier(distance);
        if(!outlier &&  distance != 65535){
          sensor_trigger = filtered_dist>560;
          if(filtered_dist > 280){
            speed_fwd = 50;
          }
        }

      if (sensor_trigger || (millis() - _forward_checkpoint) > _forward_duration)
      {
        if (sensor_trigger)
        {
          Serial.println("Stop triggered by sensor");
        }
        Serial.println("Stopped.");
        speed_fwd = 0;
        _state = 2;
      }
      #if !DIGITAL_TWIN
      motor.DC_MOVE(speed_fwd);
      #else
      SendFakeMotor(speed_fwd);
      #endif
      break;
    case 2: // wait
      if (_wait_checkpoint == 0)
      {
        GetStatus();
        _wait_checkpoint = millis();
      }
      #if !DIGITAL_TWIN
      motor.DC_STOP();
      #else
      SendFakeMotor(0);
      #endif
      if ((millis() - _wait_checkpoint) > _wait_duration)
      {
        _state = 3;
      }
      break;
    case 3: // retract
      if (_retract_checkpoint == 0)
      {
        GetStatus();
        for(int i=0; i<5; i++){
            buzzerNotify.Trigger();
            //If kalman filter is not initialized, add the measurement to the buffer
            if(!_kf.initialized){
              _kf.update(distanceSensor.readDistance());
              delay(50);
              _kf.update(distanceSensor.readDistance());
              delay(50);
            }
          }
        _retract_checkpoint = millis();
      }
      // Sensor and time logic comes first
      #if !DIGITAL_TWIN
      distance = distanceSensor.readDistance();
      Serial.println(distance);
      #else
      distance = GetFakeDistance();
      SendFakeDistanceData(distance);
      #endif
      sensor_trigger = distance < 90 && distance != 65535;
      if (sensor_trigger)
      {
        for (int i = 0; i < 3; i++)
        {
          #if !DIGITAL_TWIN
          distance += distanceSensor.readDistance();
          #else
          distance += GetFakeDistance();
          SendFakeDistanceData(distance);
          #endif
        }
        sensor_trigger = (distance / 3) < 90;
      }
      else
        retract_sensor_checks = 0;
      if (sensor_trigger || (millis() - _retract_checkpoint) > _retract_duration)
      {
        if (sensor_trigger)
        {
          Serial.println("Stop triggered by sensor");
        }
        Serial.println("Stopped.");
        #if !DIGITAL_TWIN
        motor.DC_STOP();
        #else
        SendFakeMotor(0);
        #endif
        _state = 4;
      }
      else
      {
        // Move back logic comes second
        #if !DIGITAL_TWIN
        motor.DC_MOVE(-50);
        #else
        SendFakeMotor(-50);
        #endif
      }
      break;
    case 4: // complete
      #if !DIGITAL_TWIN
      motor.DC_STOP();
      #else
      SendFakeMotor(0);
      #endif
      break;
    case 5: // paused
      #if !DIGITAL_TWIN
      motor.DC_STOP();
      #else
      SendFakeMotor(0);
      #endif

      break;
    }
  };
  void Reset()
  {
    _state = 0;
    _forward_checkpoint = 0;
    _wait_checkpoint = 0;
    _retract_checkpoint = 0;
    _warn = false;
    sensor_trigger = false;
  };
  void Retract()
  {
    if (_state == 0 || _state == 2)
      _state = 3;
  }
  String GetStatus()
  {
    return message[_state];
  };
  int GetStatusInt()
  {
    return _state;
  };
};
Deployment deployment;
int prev_state = -1;
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
        altTrigger.Reset();
        
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
      }
      else if (value_str == "STATUS")
      {
        String sts = deployment.GetStatus();
        String stat = "DEPLOY-STATUS:" + sts;
        std::string stat_std = stat.c_str(); // Convert Arduino String to std::string
        pCharacteristic->setValue(stat_std); // Set the value using std::string
      }
    }
  }
};


class AutomatedTelemetry
{
  private:
    int _repeat_status = 0; // 0: no repeat, 1: repeat all, 2: repeat altitude, 3: repeat distance, 4: repeat status
    uint32_t _last_repeat = 0;
    uint32_t _repeat_interval = 1000;
  public:
    AutomatedTelemetry(uint32_t repeat_interval){
      _repeat_interval = repeat_interval;
    }
    void SetRepeatStatus(int status){
      _repeat_status = status;
    }
    void Handle(){
      if(_repeat_status == 0) return;
      //Check for repeat interval
      if(millis()-_last_repeat < _repeat_interval) return;
      float altitude;
      uint16_t distance;
      int status;
      switch(_repeat_status){
        case 1:
          altitude = barometer.get_last_altitude_reading();
          distance = distanceSensor.readDistance();
          status = deployment.GetStatusInt();
          lora.beginPacket();
          lora.sendChar("IR");
          lora.sendFloat(altitude);
          lora.sendInt(distance);
          lora.sendInt(status);
          lora.endPacket();
          break;
        case 2:
          altitude = barometer.get_last_altitude_reading();
          lora.beginPacket();
          lora.sendChar("AR");
          lora.sendFloat(altitude);
          lora.endPacket();
          break;
        case 3:
          distance = distanceSensor.readDistance();
          lora.beginPacket();
          lora.sendChar("LI");
          lora.sendInt(distance);
          lora.endPacket();
          break;
        case 4:
          status = deployment.GetStatusInt();
          lora.beginPacket();
          lora.sendChar("DS");
          lora.sendInt(status);
          lora.endPacket();
          break;

        default:
          break;
      }
      _last_repeat = millis();
    }
};

AutomatedTelemetry autoTelemetry(1000);

void setup()
{
#if TEST_MOTOR
    deployment.TriggerProcedure();
  #if TEST_MOTOR_BACK
    deployment.Retract();
  #endif
#endif
  Serial.begin(115200);
  Serial.setRxBufferSize(1024);
  Wire.begin();
  // LORA SETUP
  lora.begin();

  buzzerNotify.Setup();
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

  distanceSensor.begin();
  distanceSensor.begin();
  otaUpdater.Setup();

  // Distance sensor setup
  delay(500);
  lora.sendSingleStr("AWAKE");
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
  static float altitude;
  static bool valid_value=true;
#if DIGITAL_TWIN
  altitude = GetFakeAltitude();
    
#else
  altitude = barometer.get_last_altitude_reading();
  altimeter_latest = altitude;
#endif
  valid_value = altTrigger.CheckAltitude(altitude);
  int descending = altTrigger.state;
  if(prev_state != descending){
    prev_state = descending;
    lora.beginPacket();
    lora.sendChar("DS");
    lora.sendInt(descending);
    lora.endPacket();
  }
#if DEBUG_ALT
  Serial.println("Altitude: "+ String(altitude)+" | Max: "+String(altTrigger.GetMaxAltitude()) + "| State: "+String(descending)+"| Outlier:"+String(!valid_value));
#endif

#if DIGITAL_TWIN
  SendAltitudeData(altitude, altTrigger.GetMaxAltitude(), descending, !valid_value);
#endif

  if (descending == 3 && deployment.GetStatus() !="FORWARD")
  {
    deployment.Deploy();
  }
  else if (descending == 4 && deployment.GetStatus() !="RETRACTING")
  {
      if (deployment.GetStatus() == "FORWARD")
      { // In case we haven't finished extended by the time we reach the lower altitude
        deployment.Stop();
      }
      deployment.Retract();
  }

  // Deployment Procedure Constant Check
  deployment.ProcedureCheck();
  
  int address, length, rssi, snr;
  byte *data;
  bool lora_available = lora.read(&address, &length, &data, &rssi, &snr);
  if (lora_available && length > 0 && lora.checkChecksum(data, length)) // A command is typically 2 bytes
  {
    /*
      |Command|Definition|Response|Definition|
      |---|---|---|---|
      |`PI`|Ping|`PO`|Pong|
      |`AS`|Altitude Single|`AS{Altitude-4B}`|Altitude Data|
      |`AR`|Altitude Repeat|`AR:R`|Altitude Repeat Received|
      |||`AR{Altitude-4B}`|Altitude Data|
      |`AWTR{H1-4B}{H2-4B}{H3-4B}`|Write altitude thresholds|`AWTR:R`|Thresholds written|
      |||`TW:F`|Failed to set thresholds|
      |`AT`|Get altitude thresholds|`AT{H1-4B}{H2-4B}{H3-4B}`|Thresholds data|
      |`DPLY`|Deploy|`DPLY:R`|Deploy Received|
      |`DS`|Deploy Status|`DS{Status-2B}`|Deploy Status Data|
      |`DSTP`|Deploy Stop|`DSTP:R`|Deploy Stop Received|
      |`DRST`|Deploy Reset|`DRST:R`|Deploy Reset Received|
      |`DRTC`|Deploy Retract|`DRTC:R`|Deploy Retract Received|
      |***Unprompted***|---|`DS{Status-2B}`|Deploy Status Data|
      |`LI`|Distance sensor|`LI{Distance-2B}`|Distance sensor data|
      |`IS`|All info single|`IS{Altitude-4B}{Distance-2B}{Status-2B}`|All info data|
      |`IR`|All info repeat|`IR:R`|All info repeat received|
      |||`SR{Altitude-4B}{Distance-2B}{Status-2B}`|All info data|
      |`JFWD`|Jog Forward|`JFWD:R`|Jog Forward Received|
      |`JREV`|Jog Reverse|`JREV:R`|Jog Reverse Received|
      |`{random command}`|Not handled (n bytes)| `NH{Command-nB}`|Not handled command|
    */
    if (lora.matchBytes(data,length,"PI", 0)){ // Ping
      lora.sendSingleStr("PO");
    }
    else if(length>1 && data[0]=='I'){
      if(data[1]=='S'){
        lora.beginPacket();
        lora.sendChar("IS");
        lora.sendFloat(altimeter_latest);
        lora.sendInt(distanceSensor.readDistance());
        lora.sendInt(deployment.GetStatusInt());
        lora.endPacket();
      }
      else if(data[1]=='R'){
        lora.sendSingleStr("IR:R");
        autoTelemetry.SetRepeatStatus(1);
      }
    }
    else if(length>1 && data[0]=='A'){
      //Altitude requests
      if(data[1]=='S'){ //AS
        lora.beginPacket();
        lora.sendChar("AS");
        lora.sendFloat(altimeter_latest);
        lora.endPacket();
      }
      else if (data[1]=='R'){ //AR
        lora.sendSingleStr("AR:R");
        autoTelemetry.SetRepeatStatus(2);
      }
      else if(data[1]=='H'){ //AH
        float* thresholds = altTrigger.GetThresholds();
        lora.sendChar("AH");
        lora.beginPacket();
        lora.sendFloat(thresholds[0]);
        lora.sendFloat(thresholds[1]);
        lora.sendFloat(thresholds[2]);
        lora.endPacket();
      }
      else if(lora.matchBytes(data,length, "WTR", 1)){ //AWTR
        //3 float values, each 4 bytes, 4 bytes for command
        bool succ = false;
        float new_trsh[3];
        try{
          if(length >= 16){
            bool h1 = lora.bytesToFloat(data, 4, &new_trsh[0]);
            bool h2 = lora.bytesToFloat(data, 8, &new_trsh[1]);
            bool h3 = lora.bytesToFloat(data, 12, &new_trsh[2]);
            succ = h1 && h2 && h3;
          }
        } catch(String error){
          succ = false;
        }
        if(succ){
          lora.sendSingleStr("AWTR:R");
          altTrigger.UpdateThresholds(new_trsh[0], new_trsh[1], new_trsh[2]);
        }
        else{
          lora.sendSingleStr("TW:F");
        }
      }
    }
    else if (length>1 && data[0]=='D')
    {
      if(lora.matchBytes(data, length, "PLY", 1)){ //DPLY
        lora.sendSingleStr("DPLY:R");
        deployment.Deploy();
      }
      else if(lora.matchBytes(data,length, "STP", 1)){ //DSTP
        lora.sendSingleStr("DSTP:R");
        deployment.Stop();
      }
      else if(lora.matchBytes(data,length, "RST", 1)){ //DRST
        lora.sendSingleStr("DRST:R");
        deployment.Reset();
        altTrigger.Reset();
      }
      else if(lora.matchBytes(data,length, "RTC", 1)){ //DRTC
        lora.sendSingleStr("DRTC:R");
        deployment.Retract();
      }
      else if(data[1]=='S'){ //DS
        if(length >2 && data[2]=='R'){
          lora.sendSingleStr("DS:R");
          autoTelemetry.SetRepeatStatus(4);
        }
        else{
          lora.beginPacket();
          lora.sendChar("DS");
          lora.sendInt(deployment.GetStatusInt());
          lora.endPacket();
        }
      }
    }
    else if (lora.matchBytes(data,length, "LI"))
    {
      lora.beginPacket();
      lora.sendChar("LI");
      lora.sendInt(distanceSensor.readDistance());
      lora.endPacket();
    }
    else if(data[0]=='J'){
      if(lora.matchBytes(data,length, "FWD", 1)){ //JFWD
        lora.sendSingleStr("JFWD:R");
        motor.DC_MOVE(50);
        delay(700);
        motor.DC_STOP();
      }
      else if(lora.matchBytes(data,length, "REV", 1)){ //JREV
        lora.sendSingleStr("JREV:R");
        motor.DC_MOVE(-50);
        delay(700);
        motor.DC_STOP();
      }
    }
    else if(lora.matchBytes(data,length, "RS")){
      //Stop any automated telemetry
      autoTelemetry.SetRepeatStatus(0);
      lora.sendSingleStr("RS:R");
    }
    else { //Not handled
      lora.beginPacket();
      lora.sendChar("NH");
      //Loop through the data except the last 2 bytes
      for(int i = 0; i < length-2; i++){
        lora.sendByte(data[i]);
      }
      lora.endPacket();
    }
  }
  // Vital Sign Indicator
  autoTelemetry.Handle();
  lora.handleQueue();
  buzzerNotify.Check();
  otaUpdater.Handle();
}
