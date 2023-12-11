#include <SoftwareSerial.h>

#define RX 3
#define TX 2
SoftwareSerial lora(RX, TX); // RX, TX --> physically(RX=2, TX=3)

void setup() {
  Serial.begin(115200); // Initialize USB Serial
  lora.begin(115200); // Initialize Software Serial
  sendATcommand("AT+ADDRESS=1", 500);
  sendATcommand("AT+BAND=902000000", 500);
  sendATcommand("AT+NETWORKID=5", 500);  
  delay(5000);
}
uint32_t count = 0;
boolean sender=true;
void loop() {
  // Check if data is available on USB Serial
  String input="GPS";
  count ++;
  // Check if data is available on Software Serial
  if (sender){
    send_command(input);
    sender=false;
    count = 0;
  }
  else if(count  >= 100000){
    sender = true;    
  }
  if (!sender){
    String incomingString;
    if (lora.available()) {
      incomingString = lora.readString();
      Serial.println(incomingString);

      char dataArray[30]; 
      incomingString.toCharArray(dataArray,30);
      char* data = strtok(dataArray, ",");
      data = strtok(NULL, ",");
      data = strtok(NULL, ",");
      Serial.println(data);
      delay(2000);
      sender=true;
   }
  }
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
       char tempArray[14+len]="AT+SEND=7,";
       strcat(tempArray,char(len));
       strcat(tempArray, returnedStr);
       Serial.println(tempArray);
       sendATcommand(tempArray, 500);
     }
}

//  $GNRMC
// time is 10 chars long starting after 7 characters. Ends at 17 chars
//
String RaspberryIncomingStr(){
    //int strLength=inputStr.length();
    return "GPS";
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

/*for char in dataArray(){
        tempArray.add(char);
        counter+=1;
        if counter==6:
           if tempArray.toString()="$GNRMC"{
              //execute action
           }else if{
              break;
           }*/
