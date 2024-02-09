#include "soar_barometer.h"
SOAR_BAROMETER bmu;
void setup(){
   Serial.begin(115200);

  bmu.Initialize();

}
void loop(){
Serial.println(bmu.get_speed_reading());
}