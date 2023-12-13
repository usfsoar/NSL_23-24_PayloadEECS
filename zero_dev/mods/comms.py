import re
import time 
import smbus


# Replace 8 with the I2C address of your Arduino
arduino_address = 0x08


def receiveMessage(log=False):
    try:
        bus = smbus.SMBus(1)  # 1 indicates /dev/i2c-1

        # Read the length of the message first
        length = bus.read_i2c_block_data(arduino_address, 0, 1)[0]

        # Read the actual message
        message = bus.read_i2c_block_data(arduino_address, 0, length)

        # Remove all non-printable characters using a regular expression
        cleaned_message = re.sub(r'[^\x20-\x7E]', '', ''.join([chr(byte) for byte in message]))

        if log:
            print(f'I2C MSG: {cleaned_message}')

        return cleaned_message
    except Exception as e:
        print(f"Slave read error: {e}")
        return ""




def sendAcknowledge(msg):
    try:
        bus = smbus.SMBus(1)  # 1 indicates /dev/i2c-1
        encoded = [ord(c) for c in msg]
        bus.write_i2c_block_data(arduino_address, 0, encoded)
    except Exception as e:
        print(f"Error: {e}")

if __name__=="__main__":
    while True:
        received_message = receiveMessage()
        if received_message is not None:
            print(f"Received message: {received_message}")
            sendAcknowledge()
        time.sleep(1)  # Adjust sleep time as needed
