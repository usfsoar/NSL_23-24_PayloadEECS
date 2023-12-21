from bluepy import btle
import time
    
# Bluetooth device MAC address
# device_mac = "DC:54:75:D8:EC:01"
device_mac = "DC:54:75:D7:A0:99"

# Service and characteristic UUIDs
service_uuid = "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
char_uuid = "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

# Create a flag to indicate when to stop the loop
stop_flag = False
awaiting_res = False

peripheral = None
characteristic = None
stop_str = "OK"

class MyDelegate(btle.DefaultDelegate):
    def handleNotification(self, cHandle, data):
        global stop_str
        received_message = data.decode('utf-8')
        print(f"Notification received on handle {cHandle}: {received_message}")
        if received_message == stop_str and awaiting_res:
            global stop_flag
            stop_flag = True

def connect_ble():
    global peripheral, characteristic
    try:
    # Connect to the BLE device
        peripheral = btle.Peripheral(device_mac)
        peripheral.setDelegate(MyDelegate())

        # Get the service and characteristic
        service = peripheral.getServiceByUUID(service_uuid)
        characteristic = service.getCharacteristics(char_uuid)[0]
        return True
    except Exception as e:
        print("Error connecting to BLE:", e)
        return False
    
def send_command(command):
    global characteristic
    try:
        characteristic.write(command.encode('utf-8'), withResponse=True)
        return True
    except Exception as e:
        print(f'Error writing to BLE: {e}')
        return False

def send_deploy():
    send_command("DEPLOY")

def send_deploy_with_res():
    return send_with_res("DEPLOY", "OK", timeout = 3.0)

def send_with_res(command, expected_res, timeout = 5.0):
    global stop_str, stop_flag, awaiting_res
    start_time = time.time()
    stop_str = expected_res
    awaiting_res = True
    while not stop_flag and (time.time()-start_time <= timeout):
        try:
            conn = send_command(command)
            if not conn:
                print("Attempting to reconnect")
                connect_ble()
            if peripheral.waitForNotifications(1.0):
                continue
            print("Waiting for response...")
            time.sleep(0.5)
        except Exception as e:
            print(f'Exception send_with_res: {e}')
    succ = True if stop_flag else False
    awaiting_res = False
    stop_flag = False
    return succ

# Disconnect from the BLE device
def disconnect():
    global peripheral
    peripheral.disconnect()

if __name__ == "__main__":
    connect_ble()
    succ = send_deploy_with_res()
    print(f'Result{succ}')
    disconnect()