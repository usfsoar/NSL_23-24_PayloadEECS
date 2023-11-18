#!/usr/bin/python

import RPi.GPIO as GPIO
import smbus

slaveAddress = 0x12    
numberInterruptPIN = 19
messageInterruptPIN = 12

def readMessageFromArduino():
    global smsMessage
    data_received_from_Arduino = i2c.read_i2c_block_data(slaveAddress, 0,15)
    for i in range(len(data_received_from_Arduino)):
        smsMessage += chr(data_received_from_Arduino[i])

    print(smsMessage.encode('utf-8'))
    data_received_from_Arduino =""
    smsMessage = ""

def readNumberFromArduino():
    global smsNumber
    data_received_from_Arduino = i2c.read_i2c_block_data(slaveAddress, 0,15)
    for i in range(len(data_received_from_Arduino)):
        smsNumber += chr(data_received_from_Arduino[i])

    print(smsNumber.encode('utf-8'))
    data_received_from_Arduino = ""
    smsNumber = ""

smsMessage = ""
smsNumber = ""

if __name__ == '__main__':
    GPIO.setmode(GPIO.BCM)
    GPIO.setup(numberInterruptPIN, GPIO.IN)
    GPIO.setup(messageInterruptPIN, GPIO.IN)

    i2c = smbus.SMBus(1)

    GPIO.add_event_detect(numberInterruptPIN, GPIO.RISING)
    GPIO.add_event_detect(messageInterruptPIN, GPIO.RISING)

    while 1:
        try:
            if GPIO.event_detected(numberInterruptPIN): 
                try:
                    readMessageFromArduino() 
                except IOError:
                    pass
            if GPIO.event_detected(messageInterruptPIN):
                try:
                    readNumberFromArduino()
                except IOError:
                    pass

        except KeyboardInterrupt:
               GPIO.cleanup()