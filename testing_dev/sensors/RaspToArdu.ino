#include <Wire.h>

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
    Wire.begin(SLAVE_ADDRESS);
    Wire.onRequest(sendData);
    Wire.onReceive(receiveData);
    delay(1000);
}

void loop() {
  if(receiveData()){
    send_command(receiveData());
  }
}

void sendData() {
  Wire.write(output);
}

char receiveData()
{
  int i=0;
   while (Wire.available() > 0) {
    char receivedChar = Wire.read();
    i++;
    if(i<=1) continue;
    
    Serial.print(receivedChar);
    return receivedChar;
    
  }
  Serial.println();
}

void send_command(String inputString){
  int len=inputString.length();
  Serial.println(inputString);
  char returnedStr[len];
  inputString.toCharArray(returnedStr,len+1);
  Serial.println(returnedStr);
  if (len<=9){
    char tempArray[12+len];
    sprintf(tempArray,"AT+SEND=7,%d,",len);
    strcat(tempArray, returnedStr);
    Serial.println(tempArray);
    sendATcommand(tempArray, 500);
  }else if (len>9 && len<=99){
    char tempArray[13+len];
    sprintf(tempArray,"AT+SEND=7,%d,",len);
    strcat(tempArray, returnedStr);
    Serial.println(tempArray);
    sendATcommand(tempArray, 500);
  }else{
    char tempArray[14+len];
    sprintf(tempArray,"AT+SEND=7,%d,",len);
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