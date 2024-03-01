#ifndef LORA_H
#define LORA_H

#include <Arduino.h>
#include <HardwareSerial.h>
#include <queue>

class SOAR_Lora {
public:
    SOAR_Lora(String address, String network_id, String frequency);
    void begin();
    void sendCommand(String command);
    void queueCommand(String command);
    void handleQueue();
    bool available();
    String read();
    String sendATCommand(const char* command, unsigned long timeout = 500);

private:
    HardwareSerial* loraSerial;
    std::queue<String> messageQueue;
    unsigned long lastSentTime;
    static const unsigned long MIN_QUEUE_TIME = 2000;
    String address;
    String network_id;
    String frequency;

    void loraSend(const char* toSend, unsigned long timeout = 500);
};

#endif // LORA_H
