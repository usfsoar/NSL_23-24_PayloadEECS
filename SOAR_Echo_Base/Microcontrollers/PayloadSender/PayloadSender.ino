#include "SOAR_Lora.h"

#define RX 3
#define TX 2

SOAR_Lora lora("6", "5", "905000000"); // LoRa
bool reporting_lock = false;
String lora_input="";
String address="";
uint32_t latency_checkpoint = 0;
void setup() {
  Serial.begin(115200); // Initialize USB Serial
  lora.begin();
  delay(1000);
}
uint32_t count = 0;
boolean sender=true;
void loop() {
  
  checkUserInput();
  count ++;
  
  if (lora_input && address != ""){
    lora.sendSingleStr(lora_input.c_str(), address.toInt());
    latency_checkpoint = millis();
    if(!reporting_lock){
      lora_input = "";
      address="";
    }
    sender=false;
    count = 0;
  }
  else{
    
    if(lora.available()){
      int address, length, rssi, snr;
      byte *data;
      bool valid_data = lora.read(&address, &length, &data, &rssi, &snr) && lora.checkChecksum(data, length);
      
      if(latency_checkpoint != 0){
        Serial.print("Latency: ");
        Serial.println(millis() - latency_checkpoint);
        latency_checkpoint = 0;
      }
     //Send the data over serial as bytes formatted like: <LORA>address;length;data;rssi;snr</LORA>
      Serial.print("<LORA>");
      //Send whether the data is valid as byte
      Serial.print(valid_data);
      Serial.print(";");
      Serial.print(address);
      Serial.print(";");
      Serial.print(length);
      Serial.print(";");
      for (int i = 0; i < length; i++) {
        Serial.print(data[i]);
      }
      Serial.print(";");
      Serial.print(rssi);
      Serial.print(";");
      Serial.print(snr);
      Serial.println("</LORA>");
      
   }
  }
}

void checkUserInput() {
  if (Serial.available() > 0) {
    String userInput = Serial.readStringUntil('\n'); // Read the input until newline character

    // Process user input
    if (userInput.length() > 0) {
      int commaIndex=userInput.indexOf(","); 
      lora_input = userInput.substring(0,commaIndex);
      address=userInput.substring(commaIndex+1);
      sender = true;
      // Check if the input ends with ":repeat"
      if (userInput.endsWith(":repeat")) {
        // Extract the part before ":"
        int colonIndex = userInput.indexOf(':');
        if (colonIndex != -1) {
          String prefix = userInput.substring(0, colonIndex);
          lora_input = prefix;
          // Set reporting_lock to true
          reporting_lock = true;
          // Use 'prefix' variable as needed
        }
      }
      else{
        reporting_lock = false;
      }
    }
  }
}