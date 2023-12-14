import serial
import platform
from threading import Thread, Event
import time

arduino = None

def connect():
    arduino_port = None
    if platform.system() == "Windows":
        # Update the COM Port to whatever port the Arduino is connected to
        arduino_port = "COM4"  # for Windows

    else:
        arduino_port = "/dev/ttyACM0"  # for Linux

    baudrate = 9600

    global arduino
    arduino = serial.Serial(arduino_port, baudrate, timeout=1)

def trigger_deploy():
    #Will basically communicate via UART
    arduino.write((bytes("Send_RF", 'utf-8')))
    print("Sending RF signal ....")

data = []
def receive_data():
    #Will receive data and then print it

    # https://stackoverflow.com/a/52240733
    while True:
        cc = str(arduino.readline())
        value = cc[2:][:-5]
        print(value)
        data.append(value)
        # return (value)
    


def send_random():
    #Will basically communicate via UART
    arduino.write((bytes("Send_Random", 'utf-8')))
    print("Sending RF signal ....")

if __name__ == "__main__":
    connect()
    receive_data()