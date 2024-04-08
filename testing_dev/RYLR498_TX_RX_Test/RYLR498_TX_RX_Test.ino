// #include "SOAR_Lora.h"



void loraSendStr(const char* toSend, unsigned long timeout) {
    loraSend((const byte*)toSend, strlen(toSend), timeout);
}

void loraSend(const byte* toSend, int length, unsigned long timeout) {

  #if !DIGITAL_TWIN
    for (int i = 0; i < 3; i++) {
        String response = sendATCommand(toSend, length, timeout);
        if (response.indexOf("+OK") >= 0) {
            break;
        } else if (response.indexOf("+ERR") >= 0) {
            Serial.println("Error response detected. Retrying...");
        } 
    }
    
  #else
    //Send lora message as bytes on the regular serial port
    //Request header 0x01 followed by 0x02, followed by the length of the message, followed by the message
    int len = strlen(toSend);
    Serial.write(0x10);
    Serial.write(0x02);
    Serial.write(len);
    //Send all the message bytes
    for (int i = 0; i < len; i++) {
        Serial.write(toSend[i]);
    }
  #endif
}

void sendATCommand(const byte* command, int length, unsigned long timeout) {
    String result;
    Serial.print("Sending: ");
    for (int i = 0; i < length; ++i) {
        // Serial.write(command[i]);
        //print the bytes as characters
        Serial.print((char)command[i]);
    }
    Serial.println();
    loraSerial->write(command, length);
    loraSerial->println();  // Ensure the command is terminated

    unsigned long startTime = millis();
    uint32_t last_read_time = 0;
    Serial.print("Received: ");
    while (millis() - startTime < timeout) { // Wait for a response
        if (loraSerial->available()) {
            char c = loraSerial->read();
            Serial.write(c);
            result += c;  // append to the result string
            last_read_time = millis();
        }
        if(last_read_time != 0 && millis() - last_read_time > 100) {
            break; // A response came in, but it's been too long since the last character
        }
    }
    Serial.println();  // new line after timeout.
    return result;
}


void setup() {
  // put your setup code here, to run once:
  loraSerial = &Serial1;
  loraSerial->begin(115200);

  loraSendStr("AT+ADDRESS=6");
  loraSendStr("AT+BAND=490000000");
  loraSendStr("AT+NETWORKID=5");


}

void loop() {
  // put your main code here, to run repeatedly:

}
