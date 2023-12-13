from bluepy import btle
import time
# Bluetooth device MAC address
device_mac = "DC:54:75:D8:EC:01"

# Service and characteristic UUIDs
service_uuid = "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
char_uuid = "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

# Create a flag to indicate when to stop the loop
stop_flag = False

class MyDelegate(btle.DefaultDelegate):
    def handleNotification(self, cHandle, data):
        received_message = data.decode('utf-8')
        print(f"Notification received on handle {cHandle}: {received_message}")
        if received_message == "OK":
            pass
        # Check if the received message indicates that we should stop the loop
        if received_message == "STOP":
            global stop_flag
            stop_flag = True

# Connect to the BLE device
peripheral = btle.Peripheral(device_mac)
peripheral.setDelegate(MyDelegate())

# Get the service and characteristic
service = peripheral.getServiceByUUID(service_uuid)
characteristic = service.getCharacteristics(char_uuid)[0]

# Write the value to the characteristic

# Wait for notifications (if any)
while not stop_flag:
    characteristic.write("DEPLOY".encode('utf-8'), withResponse=True)
    if peripheral.waitForNotifications(1.0):
        continue
    print("Waiting for notifications...")
    time.sleep(1)

# Disconnect from the BLE device
peripheral.disconnect()
