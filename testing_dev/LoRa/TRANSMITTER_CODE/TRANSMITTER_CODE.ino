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
#define GPSECHO  true

void setup() {
  //GPS SETUP --------------------------------------------------------------------------------------------------
  // connect at 115200 so we can read the GPS fast enough and echo without dropping chars
  // also spit it out
  Serial.begin(115200);
  delay(5000);
  Serial.println("Adafruit GPS library basic parsing test!");
  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  GPS.begin(9600);
  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  // GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
  // the parser doesn't care about other sentences at this time
  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate
  // For the parsing code to work nicely and have time to sort thru the data, and
  // print it out we don't suggest using anything higher than 1 Hz
  // Request updates on antenna status, comment out to keep quiet
  GPS.sendCommand(PGCMD_ANTENNA);
  delay(1000);
  // Ask for firmware version
  mySerial.println(PMTK_Q_RELEASE);




  //TRANSMITTER SETUP -----------------------------------------------------
  //THIS IS BASED OFF OF THE TUTORIAL PROVIDED: https://projecthub.arduino.cc/mdraber/how-to-use-rylr998-lora-module-with-arduino-496504

  //This tutorial calls for a baud of 57600, but the baud can be set to the maximum rate, 115200, as that is the maximum rate of the module, and the rate that the GPS examples/tutorial called
//  Serial.begin(57600);

}   

uint32_t timer = millis();

void loop() {
  //GPS SETUP --------------------------------------------------------------------------------------------------
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
  if ((c) && (GPSECHO))
    Serial.write(c);

  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences!
    // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
    //Serial.println(GPS.lastNMEA());   // this also sets the newNMEAreceived() flag to false

    if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
      return;  // we can fail to parse a sentence in which case we should just wait for another
  }

  // approximately every 2 seconds or so, print out the current stats
  if (millis() - timer > 2000) {
    timer = millis(); // reset the timer0


    //STRAYED FROM TUTORIAL: -----------------------------------------------------------------------------------------------------
    //In the transceiver tutorial, they use Serial.println() to print the data to the "output" so the transceiver can take it
    //The GPS tutorial/example nicely fits with this and uses Serial.print() as the main method of outputting the data, but to the monitor
    //Make sure to use Serial.println because this prints it with a newline character, which would be the equivalent of typing out a command, and hitting enter afterwards
    
    // OLD -----------------------
    Serial.println(GPS.lastNMEA());
    Serial.println(GPS.)

    //The following code will print out the data to the output. This is commented out so instead, the NMEA sentence can be parsed.
    // Serial.print("\nTime: ");
    // if (GPS.hour < 10) { Serial.print('0'); }
    // Serial.print(GPS.hour, DEC); Serial.print(':');
    // if (GPS.minute < 10) { Serial.print('0'); }
    // Serial.print(GPS.minute, DEC); Serial.print(':');
    // if (GPS.seconds < 10) { Serial.print('0'); }
    // Serial.print(GPS.seconds, DEC); Serial.print('.');
    // if (GPS.milliseconds < 10) {
    //   Serial.print("00");
    // } else if (GPS.milliseconds > 9 && GPS.milliseconds < 100) {
    //   Serial.print("0");
    // }
    // Serial.println(GPS.milliseconds);
    // Serial.print("Date: ");
    // Serial.print(GPS.day, DEC); Serial.print('/');
    // Serial.print(GPS.month, DEC); Serial.print("/20");
    // Serial.println(GPS.year, DEC);

    // Serial.print("Fix: "); Serial.print((int)GPS.fix);
    // Serial.print(" quality: "); Serial.println((int)GPS.fixquality);
    // if (GPS.fix) {
    //   Serial.print("Location: ");
    //   Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
    //   Serial.print(", ");
    //   Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);
    //   Serial.print("Satellites: "); Serial.println((int)GPS.satellites);

    // }
    //   Serial.print("Speed (knots): "); Serial.println(GPS.speed);
    //   Serial.print("Angle: "); Serial.println(GPS.angle);
    //   Serial.print("Altitude: "); Serial.println(GPS.altitude);
    //   Serial.print("Antenna status: "); Serial.println((int)GPS.antenna);
    // }
  }
  




  //These are sample commands for the transceiver that I have saved for reference
  //REYAX RYLR998 TRANSCEIVER AT COMMAND SHEET: https://reyax.com//upload/products_download/download_file/LoRa_AT_Command_RYLR998_RYLR498_EN.pdf
  // //Transmit the data
  // Serial.print("AT+RESET");
  // Serial.print("AT+MODE=0");
  // Serial.print("AT+SEND=2,1,L");
  // Serial.print("AT+PARAMETER=");



}
