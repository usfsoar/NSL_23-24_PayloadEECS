import serial

serialPort = serial.Serial(port='COM9', baudrate=9600, timeout=0, parity=serial.PARITY_EVEN, stopbits=1)
size = 1024

while 1:
    data = serialPort.readline(size)

    if data:
        print(data)