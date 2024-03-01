/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-microsd-card-arduino/
  
  This sketch can be found at: Examples > SD(esp32) > SD_Test
*/

#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "SOAR_SD_CARD.h"

SOAR_SD_CARD sd_card(A1);

void setup(){
  Serial.begin(115200);
  sd_card.begin();

  sd_card.listDir("/", 0);
  sd_card.createDir( "/mydir");
  sd_card.listDir( "/", 0);
  sd_card.removeDir( "/mydir");
  sd_card.listDir( "/", 2);
  sd_card.writeFile( "/hello.txt", "Hello ");
  sd_card.appendFile( "/hello.txt", "World!\n");
  sd_card.readFile( "/hello.txt");
  sd_card.deleteFile( "/foo.txt");
  sd_card.renameFile( "/hello.txt", "/foo.txt");
  sd_card.readFile( "/foo.txt");
  sd_card.testFileIO( "/test.txt");
  Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
  Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
}

void loop(){

}