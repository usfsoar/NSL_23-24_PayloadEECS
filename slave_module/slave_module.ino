
#include <Wire.h>
#include <avr/wdt.h>
#include <SoftwareSerial.h>
#include <HardwareSerial.h>
#define RX 3
#define TX 2
uint32_t GPS_FOCUS_MAX = 80000;

SoftwareSerial lora(RX, TX); // RX, TX --> physically(RX=2, TX=3) 902 mhz band
#define SLAVE_ADDRESS 0x08
byte data_to_send = 0;
byte data_to_echo = 0;
String output="IDLE";
String command = "";

void setup() {

  Serial.begin(115200);
  lora.begin(115200); // Initialize Software Serial
  sendATcommand("AT+ADDRESS=7", 500);
  sendATcommand("AT+BAND=902000000", 500);
  sendATcommand("AT+NETWORKID=5", 500); 
  Wire.begin(SLAVE_ADDRESS);
  Wire.onRequest(sendData);
  Wire.onReceive(receiveData);

  // delay(5000);
  // delay(1000);
  // Ask for firmware version
  Serial.println("Starting DEPLOY MODULE");
  Serial.print("I2C Output: ");
  Serial.print(output);
}   

uint32_t timer = millis();
void loop() {
  // if(millis() >= timer_write_test && output!="DEPLOY"){
  //   output="DEPLOY";
  // }
  if (command != ""){
    send_command(command);
    command = "";
  }
  lora.listen();  
  String incomingString ="";
  if(lora.available()){
    Serial.print("Request Received: ");
    incomingString = lora.readString();
    delay(50);
    char dataArray[incomingString.length()];
    incomingString.toCharArray(dataArray, incomingString.length());
    char* data = strtok(dataArray,",");
    data = strtok(NULL,",");
    data = strtok(NULL, ",");
    Serial.println(data);
    String data_str = String(data);
    if(data_str =="DEPLOY"){
      output= "DEPLOY";
    }
    else if(data_str=="STOP"){
      output = "STOP";
    }
    else if(data_str=="RESET"){
      output="RESET";
    }
    else if(data_str=="RETRACT"){
      output="RETRACT";
    }
    else if (data_str=="REBOOT"){
      wdt_enable(WDTO_15MS); // Enable watchdog timer with 15ms timeout
      while (1) {}
    }
    else{
      output=data_str;
    }
  }
}
void sendData() {
  String fmt_output = output+'\n';
  const char* msg = fmt_output.c_str();
  
  // Send the length of the message first
  Wire.write(strlen(msg));
  
  // Send the actual message
  Wire.write(msg);
}


void receiveData()
{
  uint32_t i=0;
  String receivedString = "";
  while (Wire.available() > 0 && i<80000) {
    char receivedChar = Wire.read();
    i++;
    if(i<=1) continue;
    receivedString += receivedChar;
    
  }
  if (receivedString == "") return;
  Serial.println(receivedString);
  if(receivedString.length()>32 || i>=80000){
    Serial.println("\nOverlofwed");
    Wire.flush();
    //RESET ARDUINO
    wdt_enable(WDTO_15MS); // Enable watchdog timer with 15ms timeout
    while (1) {}
    return;
  }
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
    sprintf(tempArray,"AT+SEND=1,%d,",len);
    strcat(tempArray, returnedStr);
    Serial.println(tempArray);
    sendATcommand(tempArray, 500);
  }else if (len>9 && len<=99){
    char tempArray[13+len];
    sprintf(tempArray,"AT+SEND=1,%d,",len);
    strcat(tempArray, returnedStr);
    Serial.println(tempArray);
    sendATcommand(tempArray, 500);
  }else{
    char tempArray[14+len];
    sprintf(tempArray,"AT+SEND=1,%d,",len);
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