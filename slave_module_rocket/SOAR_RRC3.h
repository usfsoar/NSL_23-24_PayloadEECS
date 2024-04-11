#ifndef SOAR_RRC3_H
#define SOAR_RRC3_H
#include <HardwareSerial.h>
class SOAR_RRC3{
public:
    SOAR_RRC3(int serial_bus, int rx, int tx);
    void setup();
    void GET_ALTITUDE(float& altitude, bool& ready, bool& failed);
private:
    HardwareSerial* rrc3_serial;
    int RX_PIN;
    int TX_PIN;
};
#endif