#include <Wire.h>
#include <SoftwareSerial.h>
#include <Adafruit_GPS.h>

#define GPSECHO  true
#define RX 3
#define TX 2

SoftwareSerial lora(RX, TX); // RX, TX --> physically(RX=2, TX=3) 902 mhz band
SoftwareSerial mySerial(6, 4);
//Tells the Adafruit_GPS library that this is the serial connection that it will use
Adafruit_GPS GPS(&mySerial);

#define SLAVE_ADDRESS 0x08
byte data_to_send = 0;
byte data_to_echo = 0;
char output[]="This is a test string\n";

void setup() {
    Serial.begin(115200); // Initialize USB Serial
    lora.begin(115200); // Initialize Software Serial
    sendATcommand("AT+ADDRESS=7", 500);
    sendATcommand("AT+BAND=902000000", 500);
    sendATcommand("AT+NETWORKID=5", 500); 
    GPS.begin(9600);
    GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
    GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate
    GPS.sendCommand(PGCMD_ANTENNA);
    delay(1000);
    mySerial.print(PMTK_Q_RELEASE);

    Wire.begin(SLAVE_ADDRESS);
    Wire.onRequest(sendData);
    Wire.onReceive(receiveData);
    
    delay(1000);
}
String command = "";
uint32_t timer = millis();

void loop() {
  if (command != ""){
    send_command(command);
    command = "";
  }
  lora.listen();
  String incomingString;
  if(lora.available()){
    Serial.print("Request Received");
    incomingString = lora.readString();
    delay(50);
    char dataArray[incomingString.length()];
    incomingString.toCharArray(dataArray, incomingString.length());
    char* data = strtok(dataArray,",");
    data = strtok(NULL,",");
    data = strtok(NULL, ",");
    Serial.println(data);
    char* gps = "GPS";
    if (strcmp(data,gps) || true){
      Serial.print("Processing GPS");
      mySerial.listen();
      char c = GPS.read();
      // if you want to debug, this is a good time to do it!
      if ((c) && (GPSECHO))
        Serial.write(c);
      
      if (GPS.newNMEAreceived()) {
        if (!GPS.parse(GPS.lastNMEA())){   // this also sets the newNMEAreceived() flag to falsef
          Serial.println("Failed to parse");
          return;  // we can fail to parse a sentence in which case we should just wait for another
        }
      }

      // approximately every 2 seconds or so, print out the current stats
      if (millis() - timer > 2000) {
        timer = millis(); // reset the timer

        // OLD -----------------------
        Serial.println(GPS.lastNMEA());
        String gps_response = strcat("GPS:", GPS.lastNMEA());
        Serial.print(GPS.lon);
        // String gps_response = "GPS:0000000000000000000000000000000000000000000000000000000000000000000000";
        lora.listen();
        send_command(gps_response);
        Serial.println("That's it");
      }
  }
  }
  //if(receiveData()){
  //  send_command(receiveData());
  //}
}

void sendData() {
  Wire.write(output);
}

void receiveData()
{
  Serial.print("I2C DATA");
  int i=0;
  String receivedString = "";
  while (Wire.available() > 0) {
    char receivedChar = Wire.read();
    i++;
    if(i<=1) continue;
    
    Serial.print(receivedChar);
    receivedString += receivedChar;
    //return receivedChar;
    
  }
  Serial.println(receivedString);
  command = receivedString;
  // send_command(receivedString);
  return;
}

void send_command(String inputString){
  int len=inputString.length();
  Serial.println(inputString);
  char returnedStr[len];
  inputString.toCharArray(returnedStr,len+1);
  Serial.println(returnedStr);
  if (len<=9){
    char tempArray[12+len];
    sprintf(tempArray,"AT+SEND=3,%d,",len);
    strcat(tempArray, returnedStr);
    Serial.println(tempArray);
    sendATcommand(tempArray, 500);
  }else if (len>9 && len<=99){
    char tempArray[13+len];
    sprintf(tempArray,"AT+SEND=3,%d,",len);
    strcat(tempArray, returnedStr);
    Serial.println(tempArray);
    sendATcommand(tempArray, 500);
  }else{
    char tempArray[14+len];
    sprintf(tempArray,"AT+SEND=3,%d,",len);
    strcat(tempArray, returnedStr);
    Serial.println(tempArray);
    sendATcommand(tempArray, 500);
  }
}

String sendATcommand(const char *toSend, unsigned long milliseconds) {
  String result;
  Serial.print("Sending: ");
  Serial.println(toSend);
  lora.println(toSend);
  unsigned long startTime = millis();
  Serial.print("Received: ");
  while (millis() - startTime < milliseconds) {
    if (lora.available()) {
      char c = lora.read();
      Serial.write(c);
      result += c;  // append to the result string
    }
  }
  Serial.println();  // new line after timeout.
  return result;
}