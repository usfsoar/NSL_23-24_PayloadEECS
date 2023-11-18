import smbus
import time

bus = smbus.SMBus(1)

address = 0x08

while True:
  data = bus.read_byte(address)
  print(type(data));
  #output=data.decode()
  #print (output)
  time.sleep(1)
