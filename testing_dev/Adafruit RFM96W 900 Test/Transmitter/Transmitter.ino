#include <RHReliableDatagram.h>
#include <RH_RF95.h>
#include <SPI.h>
#define SERVER_ADDRESS 1
RH_RF95 driver;
RHReliableDatagram manager(driver, SERVER_ADDRESS);

void setup() 
{
  Serial.begin(9600);
  while (!Serial) ; // Wait for serial port to be available
  if (!manager.init())
    Serial.println("init failed");
    Serial.println("Setting freq");
    driver.setFrequency(915.0);
    driver.setTxPower(23, false);
    driver.setModemConfig(RH_RF95::Bw31_25Cr48Sf512);
    manager.setTimeout(2000);
}

uint8_t data[] = "Pong";
uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];

void loop()
{
  if (manager.available())
  {
    uint8_t len = sizeof(buf);
    uint8_t from;
    if (manager.recvfromAck(buf, &len, &from))
    {
      Serial.print("got reply from : 0x");
      Serial.print(from, HEX);
      Serial.print(" : RSSI ");
      Serial.print(driver.lastRssi());
      Serial.print(" : ");
      Serial.println((char*)buf);

      if (!manager.sendtoWait(data, sizeof(data), from))
        Serial.println("No ACK-ACK");
    }
  }
}