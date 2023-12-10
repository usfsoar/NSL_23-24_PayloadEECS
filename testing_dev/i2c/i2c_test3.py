import smbus
import time

# Replace 8 with the I2C address of your Arduino
arduino_address = 0x08

def receiveMessage():
    try:
        bus = smbus.SMBus(1)  # 1 indicates /dev/i2c-1
        message = bus.read_i2c_block_data(arduino_address, 0, 32)  # 32 is the max length of the string
        msg = ''.join([chr(byte) for byte in message if byte != 0])
        result = ""
        for char in msg:
            if char == '\n':
                break
            result += char
        return result
    except Exception as e:
        print(f"Error: {e}")
        return None

def sendAcknowledge():
    try:
        bus = smbus.SMBus(1)  # 1 indicates /dev/i2c-1
        msg = "Alt:1456"
        encoded = [ord(c) for c in msg]
        bus.write_i2c_block_data(arduino_address, 0, encoded)
    except Exception as e:
        print(f"Error: {e}")

while True:
    sendAcknowledge()
    time.sleep(1000)
