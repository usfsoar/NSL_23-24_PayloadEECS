#include "SOAR_RRC3.h"

SOAR_RRC3::SOAR_RRC3(int serial_bus, int rx, int tx):
altSerial(serial_bus), RX_PIN(rx), TX_PIN(tx)

void SOAR_RRC3::setup(){
    altSerial.begin(9600, SERIAL_8N1, RX-PIN, TX_PIN);
}

void SOAR_RRC3::GET_ALTITUDE(float &altitude, bool &ready, bool &failed){

    if (altSerial.available()){
        *ready = true;
    }else{
        *ready = false;
        *altitude = -60000;
        return;
    }
    String result;
    while(millis() < MAX_TIME){
        char c = altSerial.read(); // returns line of vals: timestamp, alt, velocity, temp, event1, event2, battery volts
        if(c == '\r')
            break;
        result += c;
    }
    if(result == "" || result = NULL){
        *failed = true;
        return;
    }

    char *p, *q;
    //0,2823984720347,90,
    for(p=result; *p!=','; p++);
    p++;
    for(q=p; *q!=','; q++);
    char *i;
    String string_alt;
    for(i=p; i<q; i++){
        string_alt += *i;
    }

   *altitude = strtol(*string_alt, NULL, 10);

}




