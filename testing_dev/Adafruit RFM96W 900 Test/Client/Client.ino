#include <RHReliableDatagram.h>
#include <RH_RF95.h>
#include <SPI.h>

#define CLIENT_ADDRESS 101
#define SERVER_ADDRESS 1

RH_RF95 driver;
RHReliableDatagram manager(driver, CLIENT_ADDRESS);

void setup() 
{
  Serial.begin(9600);
  while (!Serial){
  }
  Serial.println("initializing");
  if (!manager.init())
    Serial.println("init failed");
    Serial.println("Setting freq");
    driver.setFrequency(915.0);
    driver.setTxPower(23, false);
    driver.setModemConfig(RH_RF95::Bw31_25Cr48Sf512);
    manager.setTimeout(2000);
}
uint8_t data[] = "Ping";
uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];

void loop()
{
  Serial.println("Pinging");
  if (manager.sendtoWait(data, sizeof(data), SERVER_ADDRESS))
  {
    uint8_t len = sizeof(buf);
    uint8_t from;   
    if (manager.recvfromAckTimeout(buf, &len, 2000, &from))
    {
      Serial.print("Reply from : 0x");
      Serial.print(from, HEX);
      Serial.print(" : RSSI ");
      Serial.print(driver.lastRssi());
      Serial.print(" : ");
      Serial.println((char*)buf);
    }
  }
  else
    Serial.println("No ACK");
  delay(5000);
}