#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>
#include <HardwareSerial.h>
//SOFTWARESERIAL LIBRARY DOCUMENTATION: https://docs.arduino.cc/learn/built-in-libraries/software-serial 
//TECHNICAL DETAILS of Adafruit GPS Featherwing: https://www.adafruit.com/product/3133#technical-details
//PARENT DOCUMENTATION LINK of Adafruit GPS Featherwing: https://learn.adafruit.com/adafruit-ultimate-gps-featherwing/downloads
//TECHNICAL TECHNICAL DOCS OF THE GPS CHIP on Adafruit GPS Featherwing: https://cdn-shop.adafruit.com/datasheets/GlobalTop-FGPMMOPA6H-Datasheet-V0A.pdf
//PINOUTS of Adafruit GPS Featherwing: https://learn.adafruit.com/adafruit-ultimate-gps-featherwing/pinouts



//HOW TO WIRE
//LOOK HERE----------------------------------------------------------------------------------------
//TX WIRE ON GPS IS BLUE. CONNECT THIS TO PIN 3 ON ARDUINO
//RX WIRE ON GPS IS GREEN. CONNECT THIS TO 2 ON ARDUINO
//-------------------------------------------------------------------------------------------------


//THOUGHTS AND NOTES:
/*
For starters, look at the following tutorial.
https://projecthub.arduino.cc/mdraber/how-to-use-rylr998-lora-module-with-arduino-496504
And the built-in example that comes with the Adafruit GPS Library GPS_Software_Serial_Parsing example, but the link is provided below:
https://github.com/adafruit/Adafruit_GPS/blob/master/examples/GPS_SoftwareSerial_Parsing/GPS_SoftwareSerial_Parsing.ino

Notice that the GPS Example takes the data, and uses Serial.print() to print the data to the the Serial connection. It's interesting that this prints data to the Serial connection, through the wires specified below.
The GPS behaves like a Serial device, and sends data through that serial connection. The transceiver, for its part, receives and sends data also through the serial connection, but is communicated with through the 
serial print functions, like Serial.println(). 

Now when we use the GPS module to send data to the Arduino, the below Arduino code will take that data and print it to the Serial connection. The Arduino IDE Serial Monitor can pick up this information and read it,
but what we can do instead is also connect the Transceiver to a second serial connection and have the Arduino print the data onto it there.

PROBLEM: The Arduino SoftwareSerial library can only listen to 1 Software Serial connection at a time. This will cause delays.
https://docs.arduino.cc/learn/built-in-libraries/software-serial
SOLUTION:
https://wiki.seeedstudio.com/Software-Serial/#:~:text=The%20Arduino%20hardware%20has%20the,seem%20to%20be%20the%20solution.
Every single Arduino board has a Hardware Serial connection available, but only 1. The Transceiver can be connected to the Hardware Serial connection pins, 0 and 1. These pins can be serialized and then using the 
.listen() function, the Arduino can be told to execute commands for specific serial connections.


*/



//Change the pin numbers to match the wiring of the GPS module to the Arduino Nano
//mySerial is a name for the serial connection that you create
//In the parentheses following, the parameters are this: (rXPin, tXPin, inverse_logic)

SoftwareSerial mySerial(6, 4);
//Tells the Adafruit_GPS library that this is the serial connection that it will use
Adafruit_GPS GPS(&mySerial);




//NOTES: REFER BACK TO THE EXAMPLE PROVIDED WITH THE ADAFRUIT GPS LIBRARY. THE EXAMPLE NAME IS GPS_SOFTWARE_SERIAL_PARSING.
//THAT EXAMPLE HAS MORE DETAILED COMMENTS TO EXPLAIN THE GPS CODE.


// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences
//Definitely consider messing with this statement to optimize code 
#define GPSECHO  false

void setup() {

  Serial.begin(115200);
  delay(5000);
  Serial.println("Adafruit GPS library basic parsing test!");
  GPS.begin(9600);

  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);

  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate

  GPS.sendCommand(PGCMD_ANTENNA);
  delay(1000);
  // Ask for firmware version
  mySerial.println(PMTK_Q_RELEASE);
}   

uint32_t timer = millis();

void loop() {
  //GPS SETUP --------------------------------------------------------------------------------------------------
  char c = GPS.read();
  if ((c) && (GPSECHO))
    Serial.write(c);

  if (GPS.newNMEAreceived()) {
    if (!GPS.parse(GPS.lastNMEA()))   
      return;  
  }
  // approximately every 2 seconds or so, print out the current stats
  if (millis() - timer > 2000) {
    timer = millis(); // reset the timer0
    Serial.print(GPS.lastNMEA());
}}
