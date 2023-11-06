import serial
import platform
from threading import Thread, Event
import time

if platform.system() == "Windows":
    arduino_port = "COM17"  # for Windows

else:
    arduino_port = "/dev/ttyACM0"  # for Linux

baudrate = 9600

arduino = serial.Serial(arduino_port, baudrate, timeout=.1)

def trigger_deploy():
    #Will basically communicate via UART
    arduino.write((bytes("Send_RF", 'utf-8')))
    print("Sending RF signal ....")
def send_random():
    #Will basically communicate via UART
    arduino.write((bytes("Send_Random", 'utf-8')))
    print("Sending RF signal ....")
