#include "SOAR_RRC3.h"

SOAR_RRC3::SOAR_RRC3(int serial_bus, int rx, int tx):
rrc3_serial(serial_bus), RX_PIN(rx), TX_PIN(tx)
{}

void SOAR_RRC3::setup(){

}

void SOAR_RRC3::GET_ALTITUDE(float& altitude, bool& ready, bool&failed){

}