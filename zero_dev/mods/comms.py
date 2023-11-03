import serial
import time 

lora = serial.Serial("/dev/ttyAMA0", baudrate=9600, timeout=1.0)

lora.write('AT+BAND=902000000\r\n')
result=lora.read(100)
print(result)

lora.write('AT+CMGF=1\r\n')
result=lora.read(100)