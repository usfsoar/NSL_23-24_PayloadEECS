import serial
import threading
import struct

# Open serial port
ser = serial.Serial('COM21', 2000000, timeout=1)

# Function to read and respond to requests
def read_serial():
    while True:
        data = ser.read()
        if data == b'\x01':  # Request code for altitude
            altitude = 69.0
            response = b'\x02' + struct.pack('f', altitude)
            ser.write(response)
        elif data:  # Non-empty data indicates a possible string message
            # Read the rest of the line as a string
            line = data + ser.readline()
            line = line.decode('utf-8').strip()
            print(line)
            # Handle other string-based commands here, if necessary

# Create and start a thread for reading serial data
thread = threading.Thread(target=read_serial)
thread.daemon = True
thread.start()

# Wait for the user to press enter
input("Press Enter to exit...\n")
