
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "buzzer_notify.h"

//TODO: Get rid of whatever this library is doing
#include "Adafruit_BMP3XX.h"
#include <HardwareSerial.h>
#include "DCMotor.h"
#include "ota_update.h"

#define RX -1
#define TX -1

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

OTA_Update otaUpdater("soar-deploy", "TP-Link_BCBD", "10673881");


//STEPPER MOTOR DELAYS
static const int microDelay = 900;
static const int betweenDelay = 250;

//DC motor
DCMotor motor(A2, 50, 50);

//LORA Variables and Objects
HardwareSerial Lora(0);
String output = "IDLE";


Adafruit_BMP3XX bmp;
void bmp_setup()
{
  Wire.begin();
  if (!bmp.begin_I2C())
  { // hardware I2C mode, can pass in address & alt Wire
    // if (! bmp.begin_SPI(BMP_CS)) {  // hardware SPI mode
    // if (! bmp.begin_SPI(BMP_CS, BMP_SCK, BMP_MISO, BMP_MOSI)) {  // software SPI mode
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
int bmp_fail = 0;
float GetAltitude()
{
  if (!bmp.performReading())
  {
    Serial.println("Failed to perform reading");
    bmp_fail++;
    if (bmp_fail > 10)
    {
      bmp_fail = 0;
      bmp_setup();
      delay(100);
    }
    // Attempt to reconnect to the sensor
    return 0;
  }
  bmp_fail = 0;
  return bmp.readAltitude(SEALEVELPRESSURE_HPA);
}

float previous_altitude = -300;
float max_candidate = -300;
int alt_trigger_count = 0;
float immediate_previous = -6000;
bool altitudeTrigger(float current_altitude)
{
#if DEBUG_ALT
  Serial.print("Dif:");
  Serial.println(current_altitude - previous_altitude);
  Serial.print("Prev:");
  Serial.println(previous_altitude);
#endif
  bool res = false;
  // Check if the altitude is decreasing and above 30.48 meters
  if ((current_altitude > ALT_TRSH_CHECK) && (current_altitude - previous_altitude < -2))
  {
    res = true;
  }
  if (current_altitude > previous_altitude)
    if (current_altitude - immediate_previous < 800 || immediate_previous == -60000)
    { // Default value for errors
      previous_altitude = current_altitude;
    }
  if (current_altitude - immediate_previous > 800)
    res = false;
  immediate_previous = current_altitude;
  return res; // NOTRIGGER
  // Update previous_altitude for the next function call
}



BuzzerNotify buzzerNotify = BuzzerNotify(buzzerPin);

class Deployment
{
private:
  bool _started = false;
  bool _active = false;
  bool _forward = false;
  bool _nimble = false;
  bool _retract = false;
  uint32_t _last_checkpoint = 0;
  uint32_t _move_duration = 10000;   // 43 seconds
  uint32_t _reset_duration = 10000;  // Around half of move duration
  uint32_t _nimble_duration = 10000; // 10 seconds
public:
  Deployment(){};
  void TriggerProcedure()
  {
    if (!_active)
    {
      _active = true;
      _last_checkpoint = millis();
    }
  };
  void Stop()
  {
    _active = false;
    motor.DC_STOP();
  };
  void ProcedureCheck()
  {
    if (!_active)
      return;
    const uint32_t curr_duration = millis() - _last_checkpoint;
    if (!_forward && curr_duration < _move_duration)
    {
      Serial.println("Deploying forward...");
      motor.DC_MOVE(50);
    }
    else if (!_forward && curr_duration >= _move_duration)
    {
      Serial.println("Stopping forward deploy");
      _forward = true;
      _last_checkpoint = millis();
    }
    else if (_forward && !_nimble && curr_duration < _nimble_duration)
    {
      Serial.println("Allowing time to deploy...");
      motor.DC_STOP();
    }
    else if (_forward && !_nimble && curr_duration >= _nimble_duration)
    {
      Serial.println("Triggering retract");
      _nimble = true;
      _last_checkpoint = millis();
    }
    else if (_forward && _nimble && !_retract && curr_duration < _move_duration)
    {
      Serial.println("Retracting back");
      motor.DC_MOVE(-50);
    }
    else if (_forward && _nimble && !_retract && curr_duration > _reset_duration)
    {
      Serial.println("Retracting completed");
      motor.DC_STOP();
      _retract = true;
    }
    if (_forward && _nimble && _retract)
    {
      _active = false;
    }
  };
  void Reset()
  {
    _forward = false;
    _nimble = false;
    _retract = false;
    _last_checkpoint = 0;
    _active = false;
  };
  void Retract()
  {
    _forward = true;
    _nimble = true;
    _retract = false;
    _last_checkpoint = millis();
    _active = true;
  }
  String GetStatus()
  {
    if (_active)
    {
      if (!_forward && !_nimble && !_retract)
      {
        return "EXTENDING";
      }
      else if (_forward && !_nimble && !_retract)
      {
        return "WAITING";
      }
      else if (_forward && _nimble && !_retract)
      {
        return "RETRACTING";
      }
      else if (_forward && _nimble && _retract)
      {
        return "COMPLETED";
      }
      else
      {
        return "PAUSED";
      }
    }
    else
    {
      if (_forward && _nimble && _retract)
      {
        return "COMPLETED";
      }
      else if (_forward || _nimble || _retract)
      {
        return "PAUSED";
      }
      else
      {
        return "IDLE";
      }
    }
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
        deployment.TriggerProcedure();
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
          // pCharacteristic->setValue(stat);
        // pCharacteristic->notify();
        // Serial.println("Rtracting deployment\n");

      }
    }
  }
};

void LoraSend(const char *toSend, unsigned long milliseconds = 500)
{
  for (int i = 0; i < 3; i++)
  {
    String res = sendATcommand(toSend, milliseconds);
    Serial.println(res);
    if (res.indexOf("+ERR") >= 0)
    {
      Serial.println("Err response detected. Retrying...");
    }
    else
    {
      break;
    }
  }
}
String sendATcommand(const char *toSend, unsigned long milliseconds)
{
  String result;
  Serial.print("Sending: ");
  Serial.println(toSend);
  Lora.println(toSend);
  unsigned long startTime = millis();
  Serial.print("Received: ");
  while (millis() - startTime < milliseconds)
  {
    if (Lora.available())
    {
      char c = Lora.read();
      Serial.write(c);
      result += c; // append to the result string
    }
  }
  Serial.println(); // new line after timeout.
  return result;
}

void send_command(String inputString)
{
  int len = inputString.length();
  Serial.println(inputString);
  char returnedStr[len];
  inputString.toCharArray(returnedStr, len + 1);
  Serial.println(returnedStr);
  if (len <= 9)
  {
    char tempArray[12 + len];
    sprintf(tempArray, "AT+SEND=1,%d,", len);
    strcat(tempArray, returnedStr);
    Serial.println(tempArray);
    LoraSend(tempArray, 500);
  }
  else if (len > 9 && len <= 99)
  {
    char tempArray[13 + len];
    sprintf(tempArray, "AT+SEND=1,%d,", len);
    strcat(tempArray, returnedStr);
    Serial.println(tempArray);
    LoraSend(tempArray, 500);
  }
  else
  {
    char tempArray[14 + len];
    sprintf(tempArray, "AT+SEND=1,%d,", len);
    strcat(tempArray, returnedStr);
    Serial.println(tempArray);
    LoraSend(tempArray, 500);
  }
}

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

  //LORA SETUP
  Lora.begin(115200, SERIAL_8N1, RX, TX);

  LoraSend("AT+ADDRESS=5", 500);
  LoraSend("AT+BAND=905000000", 500);
  LoraSend("AT+NETWORKID=5", 500);

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
  bmp_setup();
  buzzerNotify.Trigger();
  motor.DC_SETUP();
  buzzerNotify.Trigger();

  otaUpdater.Setup();

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
  float altitude = GetAltitude();
  altimeter_latest = altitude;
#if DEBUG_ALT
  Serial.print("Altitude: ");
  Serial.println(altitude);
#endif

  bool descending = altitudeTrigger(altitude);
  if (descending)
  {
    if (alt_trigger_count > 5)
    { // Must make sure that the trigger is not a false positive
      Serial.println("Triggering deployment");
      deployment.TriggerProcedure();
    }
    else
    {
      alt_trigger_count++; // This must be protected under else to prevent overflow
    }
  }

  // Deployment Procedure Constant Check
  deployment.ProcedureCheck();

  if (Lora.available())
  {
    String incomingString = "";
    Serial.print("Request Received: ");
    incomingString = Lora.readString();
    delay(50);
    char dataArray[incomingString.length()];
    incomingString.toCharArray(dataArray, incomingString.length());
    char *data = strtok(dataArray, ",");
    data = strtok(NULL, ",");
    data = strtok(NULL, ",");
    Serial.println(data);
    String data_str = String(data);
    if (data_str == "PING")
    {
      send_command("PONG");
    }
    if (data_str == "DEPLOY")
    {
      output = "DEPLOY";
      Serial.println("Deployment Triggered");
      deployment.TriggerProcedure();
      send_command("DEPLOY:TRIGGERING");
    }
    else if (data_str == "STOP")
    {
      output = "STOP";
      deployment.Stop();
      send_command("DEPLOY:STOPING");
    }
    else if (data_str == "RESET")
    {
      output = "RESET";
      deployment.Reset();
      send_command("DEPLOY:RESETING");
    }
    else if (data_str == "STATUS")
    {
      String stat = "DEPLOY-STATUS:" + deployment.GetStatus();
      send_command(stat);
    }
    else if (data_str == "RETRACT")
    {
      output = "RETRACT";
      deployment.Retract();
      send_command("DEPLOY:RETRACTING");
    }
    else if (data_str == "ALTITUDE")
    {
      char altimeter_latest_str[9];
      dtostrf(altimeter_latest, 4, 2, altimeter_latest_str);
      char altitude_str[100] = "ALTITUDE:";
      strcat(altitude_str, altimeter_latest_str);
      send_command(altitude_str);
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
        send_command("THRESHOLD:SET");
      }
      catch (String error)
      {
        send_command("THRESHOLD:ERROR");
      }
    }
    else
    {
      output = data_str;
      send_command("INVALID");
    }
  }
  // Vital Sign Indicator
  buzzerNotify.Check();
  otaUpdater.Handle();
}
