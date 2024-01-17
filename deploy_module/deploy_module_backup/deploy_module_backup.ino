
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"
#include <AccelStepper.h>
#include <HardwareSerial.h>

#define RX 3
#define TX 2

#define TEST false
#define stepPin A3
#define dirPin A2
#define motorInterfaceType 1
#define buzzerPin A10

#define SEALEVELPRESSURE_HPA (1013.25)
#define ALT_TRSH_CHECK 0 // Use -10 for parking lot test and maybe change it on location

static const int microDelay = 900;
static const int betweenDelay = 250;

HardwareSerial Lora(1);
String output = IDLE;

// Create a new instance of the AccelStepper class
AccelStepper stepper = AccelStepper(motorInterfaceType, stepPin, dirPin);

Adafruit_BMP3XX bmp;
void bmp_setup()
{
  Wire.begin();
  Serial.println("Adafruit BMP388 / BMP390 test");
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
    Serial.println("Failed to perform reading :(");
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
bool altitudeTrigger(float current_altitude)
{
  // Check if the altitude is decreasing and above 30.48 meters
  if ((current_altitude - previous_altitude < -1) && current_altitude > ALT_TRSH_CHECK)
  {
    // previous_altitude = current_altitude;
    return true;
  }
  if (current_altitude > previous_altitude)
    if (current_altitude <= 1731 || current_altitude >= 1732)
    { // Default value for errors
      previous_altitude = current_altitude;
    }
  return false; // NOTRIGGER
  // Update previous_altitude for the next function call
}

BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;

// Function to move the motor a certain number of degrees
void moveStepper(int degrees, double vel)
{
  if (degrees == 0)
    return;
  // If degrees negative dir=0 else dir=1
  bool dir = degrees > 0 ? 1 : 0;
  if (dir)
    digitalWrite(dirPin, HIGH); // stepper.setPinsInverted(false, false, false); // Enables the motor to move in a particular direction
  else
    digitalWrite(dirPin, LOW); // stepper.setPinsInverted(false, false, true);
  // digitalWrite(dirPin, HIGH);
  int steps = round(abs(degrees) / 360.0 * 200);
  ; // Convert degrees to steps
  if (steps == 0)
    return;

  // Move the motor to the target position
  // stepper.moveTo(steps);
  // while(stepper.distanceToGo() != 0) {
  // stepper.run();
  //}
  Serial.print("Rotating steps:");
  Serial.println(steps);
  for (int i = 0; i < steps; i++)
  {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(250); // A value between 225 and 250 is the breaking point of the motor movement, meaning that value won't make the motor run.
    digitalWrite(stepPin, LOW);
    delayMicroseconds(250);
  }

  if (dir)
    digitalWrite(dirPin, LOW); // stepper.setPinsInverted(false, false, false); // Enables the motor to move in a particular direction
  else
    digitalWrite(dirPin, HIGH);
  for (int j = 0; j < 5; j++)
  {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(250);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(250);
  }
}

class BuzzerNotify
{
public:
  BuzzerNotify(int pin) : pin_number(pin){};

  void Setup()
  {
    pinMode(pin_number, OUTPUT);
  };
  void Check()
  {
    curr_cycles++;
    if (curr_cycles > MAX_CYCLES && !beeping)
    {
      digitalWrite(pin_number, HIGH);
      beeping = true;
    }
    if (curr_cycles > MAX_CYCLES_ON)
    {
      digitalWrite(pin_number, LOW);
      beeping = false;
      Reset();
    }
  };
  void Trigger()
  {
    if (!beeping)
    {
      digitalWrite(pin_number, HIGH);
      delay(50);
      digitalWrite(pin_number, LOW);
    }
  };
  void Reset()
  {
    curr_cycles = 0;
  };

private:
  int pin_number;
  bool beeping = false;
  const uint32_t MAX_CYCLES = 18000000;
  const uint32_t MAX_CYCLES_ON = 19500000;
  uint32_t curr_cycles = 0;
};
BuzzerNotify buzzerNotify = BuzzerNotify(buzzerPin);

class Deployment
{
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
  };
  void ProcedureCheck()
  {
    if (!_active)
      return;
    const uint32_t curr_duration = millis() - _last_checkpoint;
    if (!_forward && curr_duration < _move_duration)
    {
      Serial.println("Deploying forward...");
      moveStepper(360, 0.9);
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
      moveStepper(-360, 0.8);
    }
    else if (_forward && _nimble && !_retract && curr_duration > _reset_duration)
    {
      Serial.println("Retracting completed");
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

private:
  bool _started = false;
  bool _active = false;
  bool _forward = false;
  bool _nimble = false;
  bool _retract = false;
  uint32_t _last_checkpoint = 0;
  uint32_t _move_duration = 25000;   // 43 seconds
  uint32_t _reset_duration = 12500;  // Around half of move duration
  uint32_t _nimble_duration = 10000; // 10 seconds
};
Deployment deployment;

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
      }
    }
  }
};

void setup()
{
  // Set the maximum speed and acceleration
  stepper.setMaxSpeed(1000);
  stepper.setAcceleration(500);

  Lora.begin(115200, SERIAL_8N1, RX, TX);
  sendATcommand("AT+ADDRESS=5", 500);
  sendATcommand("AT+BAND=902000000", 500);
  sendATcommand("AT+NETWORKID=5", 500);

#if TEST
  deployment.TriggerProcedure();
#endif
  Serial.begin(115200);
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
  Serial.print("Altitude: ");
  Serial.println(altitude);
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
  Lora.listen();
  String incomingString = "";
  if (Lora.available())
  {
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
    if (data_str == "GPS")
    {
      gps_focus = true;
      gps_focus_cycles = 0;
    }
    else if (data_str == "DEPLOY")
    {
      output = "DEPLOY";
    }
    else if (data_str == "STOP")
    {
      output = "STOP";
    }
    else if (data_str == "RESET")
    {
      output = "RESET";
    }
    else if (data_str == "RETRACT")
    {
      output = "RETRACT";
    }
    else if (data_str == "REBOOT")
    {
      wdt_enable(WDTO_15MS); // Enable watchdog timer with 15ms timeout
      while (1)
      {
      }
    }
    else
    {
      output = data_str;
    }
  }
}
if (gps_focus)
{
  // Vital Sign Indicator
  buzzerNotify.Check();
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
    sendATcommand(tempArray, 500);
  }
  else if (len > 9 && len <= 99)
  {
    char tempArray[13 + len];
    sprintf(tempArray, "AT+SEND=1,%d,", len);
    strcat(tempArray, returnedStr);
    Serial.println(tempArray);
    sendATcommand(tempArray, 500);
  }
  else
  {
    char tempArray[14 + len];
    sprintf(tempArray, "AT+SEND=1,%d,", len);
    strcat(tempArray, returnedStr);
    Serial.println(tempArray);
    sendATcommand(tempArray, 500);
  }
}
/*

void loop(){
    moveStepper(90, 0.95);
    delay(1000);
}


*/
