    #include "SOAR_IMU.h"

SOAR_IMU imu_sensor;

void setup() {
    Serial.begin(115200);
    imu_sensor.BNO_SETUP();
    return;
}


void loop(){

    float *a = imu_sensor.GET_ACCELERATION();
    String out =  String(millis()) + " , " + String(a[0]) + " , " + String(a[1]) + " , " + String(a[2]) +"\n";
    const char * c = out.c_str();
    Serial.println(c);
    return;

}
  

