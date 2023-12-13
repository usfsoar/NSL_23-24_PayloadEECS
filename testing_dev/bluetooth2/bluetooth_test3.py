from bluepy import btle
import time
# Bluetooth device MAC address
device_mac = "DC:54:75:D8:EC:01"

# Service and characteristic UUIDs
service_uuid = "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
char_uuid = "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

# Value to be sent
value_to_send = "DEPLOY"

class MyDelegate(btle.DefaultDelegate):
    def handleNotification(self, cHandle, data):
        print(f"Notification received on handle {cHandle}: {data.decode('utf-8')}")

# Connect to the BLE device
peripheral = btle.Peripheral(device_mac)
peripheral.setDelegate(MyDelegate())

# Get the service and characteristic
service = peripheral.getServiceByUUID(service_uuid)
characteristic = service.getCharacteristics(char_uuid)[0]

# Write the value to the characteristic

# Wait for notifications (if any)
while True:
    characteristic.write(value_to_send.encode('utf-8'), withResponse=True)
    if peripheral.waitForNotifications(1.0):
        continue
    print("Waiting for notifications...")
    time.sleep(1);
