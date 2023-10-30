import serial
import time 

phone = serial.Serial("/dev/ttyAMA0", baudrate=9600, timeout=1.0)

phone.write('AT+CPIN=0000\r\n')
result=phone.read(100)
print result

phone.write('AT+CMGF=1\r\n')
result=phone.read(100)