#include "SOAR_Lora.h"

SOAR_Lora::SOAR_Lora(String address, String network_id, String frequency) :
    address(address),
    network_id(network_id),
    frequency(frequency) {}


void SOAR_Lora::begin() {
    loraSerial = new HardwareSerial(0);  // Assuming LoRa is connected to Serial0
    loraSerial->begin(115200, SERIAL_8N1, -1, -1);
    sendATCommand(("AT+ADDRESS=" + address).c_str());
    sendATCommand(("AT+BAND=" + frequency).c_str());
    sendATCommand(("AT+NETWORKID=" + network_id).c_str());
    lastSentTime = 0;
}

bool SOAR_Lora::available() {
    return loraSerial->available();
}
String SOAR_Lora::read() {
    Serial.print("Request Received: ");
    String incomingString = loraSerial->readString();
    delay(50);
    char dataArray[incomingString.length()];
    incomingString.toCharArray(dataArray, incomingString.length());
    char* data = strtok(dataArray, ",");
    data = strtok(NULL, ",");
    data = strtok(NULL, ",");
    Serial.println(data);
    String data_str = String(data);
    return data_str;
}

void SOAR_Lora::sendCommand(String command) {
    int len = command.length();
    char commandBuffer[len + 1];
    command.toCharArray(commandBuffer, len + 1);

    char sendBuffer[20 + len];  // Buffer for the AT+SEND command
    sprintf(sendBuffer, "AT+SEND=1,%d,%s", len, commandBuffer);
    loraSend(sendBuffer);
    lastSentTime = millis();
}

void SOAR_Lora::queueCommand(String command) {
    messageQueue.push(command);
    this->handleQueue();
}

void SOAR_Lora::handleQueue() {
    if (!messageQueue.empty() && (millis() - lastSentTime >= MIN_QUEUE_TIME)) {
        sendCommand(messageQueue.front());
        messageQueue.pop();
    }
}

String SOAR_Lora::sendATCommand(const char* command, unsigned long timeout) {
    String result;
    Serial.print("Sending: ");
    Serial.println(command);
    loraSerial->println(command);
    unsigned long startTime = millis();
    Serial.print("Received: ");
    while (millis() - startTime < timeout) {
      if (loraSerial->available()) {
        char c = loraSerial->read();
        Serial.write(c);
        result += c;  // append to the result string
      }
    }
    Serial.println();  // new line after timeout.
    return result;
}

void SOAR_Lora::loraSend(const char* toSend, unsigned long timeout) {
    for (int i = 0; i < 3; i++) {
        String response = sendATCommand(toSend, timeout);
        if (response.indexOf("+OK") >= 0) {
            break;
        } else if (response.indexOf("+ERR") >= 0) {
            Serial.println("Error response detected. Retrying...");
        } else {
            break;
        }
    }
}
