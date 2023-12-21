from Services import parser

import serial
import platform
from threading import Thread, Event
import time
import re
import csv
from datetime import datetime

arduino = None

def connect(port=""):
    arduino_port = port
    if arduino_port is "":
        if platform.system() == "Windows":
            # Update the COM Port to whatever port the Arduino is connected to
            arduino_port = "COM3"  # for Windows

        else:
            arduino_port = "/dev/ttyACM0"  # for Linux

    baudrate = 115200

    global arduino
    arduino = serial.Serial(arduino_port, baudrate, timeout=1)
def close():
    global arduino
    arduino.close()


def gps_repeat():
    arduino.write((bytes("GPS:repeat", 'utf-8')))
    print("Triggering GPS:repeat")

def telemetry_repeat():
    arduino.write((bytes("Telemetry:repeat", 'utf-8')))
    print("Triggering Telemetry:repeat")

def trigger_deploy():
    #Will basically communicate via UART
    arduino.write((bytes("Send_RF", 'utf-8')))
    print("Sending RF signal ....")

lora = []
data = []
def receive_data():
    #Will receive data and then print it

    # https://stackoverflow.com/a/52240733
    while True:
        cc = str(arduino.readline())
        value = cc[2:][:-5]
        print(value)
        data.append(value)
        pattern = r'^<LORA>\s*.*\s*</LORA>$'
        match = re.match(pattern, value)
        if bool(match):
            print(value)
            lora.append(value)
            parser.relay_message(value)

            # Write the data to a CSV file
            current_time = datetime.now().strftime('%m-%d-%Y %H:%M:%S')
            with open(f'{current_time}.csv', 'a', newline='') as file:
                writer = csv.writer(file)
                writer.writerow(["datetime", "serial msg"])
                writer.writerow([current_time, value])

def send_random():
    #Will basically communicate via UART
    arduino.write((bytes("Send_Random", 'utf-8')))
    print("Sending RF signal ....")

if __name__ == "__main__":
    connect()
    receive_data()