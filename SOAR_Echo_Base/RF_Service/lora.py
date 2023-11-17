import serial
import platform
from threading import Thread, Event
import time

arduino = None

def connect():
    if platform.system() == "Windows":
        arduino_port = "COM4"  # for Windows

    else:
        arduino_port = "/dev/ttyACM0"  # for Linux

    baudrate = 9600

    arduino = serial.Serial(arduino_port, baudrate, timeout=.1)

def trigger_deploy():
    #Will basically communicate via UART
    arduino.write((bytes("Send_RF", 'utf-8')))
    print("Sending RF signal ....")

def receive_data():
    #Will receive data and then print it
    current_state = "idle"
    while(True):
        if(arduino.available() > 0):
            data = arduino.read()
            if (data == "<<GPS>>"):
                current_state = "GPS"
                    
            else:
                if current_state == "GPS":
                    pass

def send_random():
    #Will basically communicate via UART
    arduino.write((bytes("Send_Random", 'utf-8')))
    print("Sending RF signal ....")
