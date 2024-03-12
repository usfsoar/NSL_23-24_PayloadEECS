import time
import os
from Config import *
from Services.serial_device import SerialDevice
from flask import Flask, render_template, jsonify, jsonify, request, send_from_directory
from threading import Thread
import re
import struct

payloadSender = None
recoverySender = None

addresses = {
    10: "Drone",
    5: "Deployment",
    7: "Recovery"
}

@app.route('/')
def control_panel():
    # render html under Views/control_panel.html
    directory = os.path.join(app.root_path,'..', 'Views', 'control_panel')
    return send_from_directory(directory, 'control_panel.html')

@app.route('/start_payload_serial/<port>')
def start_serial(port = "COM7"):
    global payloadSender
    try:
        payloadSender = SerialDevice("Payload")
        payloadSender.connect(port)
        print(f'Stablishing serial {port}')
        lora_telem_thread = Thread(target=payloadSender.receive_data)
        lora_telem_thread.daemon = True
        lora_telem_thread.start()
        time.sleep(0.5)
        message_handler_thread = Thread(target=payload_message_handler, args=(payloadSender,))
        message_handler_thread.daemon = True
        message_handler_thread.start()
        return jsonify(message = "Started Serial"), 200
    except Exception as e:
        print(f'Error connecting to serial: {e}')
        return jsonify(message = f'Error: {e}'), 500

def log_handler(device):
    message = ""
    lora_packet = None
    if device.message_queue.qsize() > 0:
        message = device.message_queue.get()
        socketio.emit('serial_log', {'device': device.name, 'message':message})
    if device.lora_packet_queue.qsize() > 0:
        lora_packet = device.lora_packet_queue.get()
        # the lora packet is of type LoraPacket send it as json
        lora_packet_json = lora_packet.to_json()
        socketio.emit('lora_log', {'device':device.name, 'packet':lora_packet_json})
    return message, lora_packet

# function to check if byte array are equal to a string
def compBytesStr(bytes, str_val, start = 0):
    #if the string is longer than the bytes at the start index then return false
    if len(str_val) > len(bytes) - start:
        return False
    #iterate through the string and compare each byte to the string starting at the start index
    for i in range(len(str_val)):
        if bytes[i + start] != ord(str_val[i]):
            return False
    return True
    
    

def payload_message_handler(device):
    # Constantly check for the queue on assigned serial device
    # If serial device closes, break the loop
    while True:
        if not device.device.is_open:
            break
        message, lora_packet  = log_handler(device)
        if lora_packet == None:
            continue
        # proceed to process the LoraPacket Response
        '''
            |Command|Definition|Response|Definition|
            |---|---|---|---|
            |`PI`|Ping|`PO`|Pong|
            |`AS`|Altitude Single|`AS{Altitude-4B}`|Altitude Data|
            |`AR`|Altitude Repeat|`AR:R`|Altitude Repeat Received|
            |||`AR{Altitude-4B}`|Altitude Data|
            |`AWTR{H1-4B}{H2-4B}{H3-4B}`|Write altitude thresholds|`AWTR:R`|Thresholds written|
            |||`TW:F`|Failed to set thresholds|
            |`AT`|Get altitude thresholds|`AT{H1-4B}{H2-4B}{H3-4B}`|Thresholds data|
            |`DPLY`|Deploy|`DPLY:R`|Deploy Received|
            |`DS`|Deploy Status|`DS{Status-2B}`|Deploy Status Data|
            |`DSTP`|Deploy Stop|`DSTP:R`|Deploy Stop Received|
            |`DRST`|Deploy Reset|`DRST:R`|Deploy Reset Received|
            |`DRTC`|Deploy Retract|`DRTC:R`|Deploy Retract Received|
            |***Unprompted***|---|`DS{Status-2B}`|Deploy Status Data|
            |`LI`|Distance sensor|`LI{Distance-2B}`|Distance sensor data|
            |`IS`|All info single|`IS{Altitude-4B}{Distance-2B}{Status-2B}`|All info data|
            |`IR`|All info repeat|`IR:R`|All info repeat received|
            |||`IR{Altitude-4B}{Distance-2B}{Status-2B}`|All info data|
            |`JFWD`|Jog Forward|`JFWD:R`|Jog Forward Received|
            |`JREV`|Jog Reverse|`JREV:R`|Jog Reverse Received|
            |`{random command}`|Not handled (n bytes)| `NH{Command-nB}`|Not handled command|
        '''
        if not lora_packet.valid_data:
            # TODO: Handle invalid data, maybe predict what it was supposed to be
            return
        data_bytes = lora_packet.data_bytes
        sender = addresses[lora_packet.address]
        #if the first 2 data bytes are "PO" then it is a pong
        if compBytesStr(data_bytes, "PO"):
            print(f"Pong from {sender}")
        elif compBytesStr(data_bytes, "AS"):
            altitude = int.from_bytes(data_bytes[2:], byteorder='big')
            print(f"Altitude from {sender}: {altitude}")
        elif compBytesStr(data_bytes, "AR"):
            altitude = int.from_bytes(data_bytes[2:], byteorder='big')
            print(f"Altitude from {sender}: {altitude}")
        elif compBytesStr(data_bytes, "AWTR:R"):
            print(f"Thresholds written to {sender}")
        elif compBytesStr(data_bytes, "AWTR:F"):
            print(f"Failed to write thresholds to {sender}")
        elif compBytesStr(data_bytes, "AT"):
            h1 = int.from_bytes(data_bytes[2:6], byteorder='big')
            h2 = int.from_bytes(data_bytes[6:10], byteorder='big')
            h3 = int.from_bytes(data_bytes[10:14], byteorder='big')
            print(f"Thresholds from {sender}: {h1}, {h2}, {h3}")
        elif compBytesStr(data_bytes, "DPLY:R"):
            print(f"Deploy received from {sender}")
        elif compBytesStr(data_bytes, "DS"):
            status = int.from_bytes(data_bytes[2:], byteorder='big')
            print(f"Deploy Status from {sender}: {status}")
        elif compBytesStr(data_bytes, "DSTP:R"):
            print(f"Deploy Stop received from {sender}")
        elif compBytesStr(data_bytes, "DRST:R"):
            print(f"Deploy Reset received from {sender}")
        elif compBytesStr(data_bytes, "DRTC:R"):
            print(f"Deploy Retract received from {sender}")
        elif compBytesStr(data_bytes, "LI"):
            distance = int.from_bytes(data_bytes[2:], byteorder='big')
            print(f"Distance from {sender}: {distance}")
        elif compBytesStr(data_bytes, "IS"):
            altitude = int.from_bytes(data_bytes[2:6], byteorder='big')
            distance = int.from_bytes(data_bytes[6:8], byteorder='big')
            status = int.from_bytes(data_bytes[8:], byteorder='big')
            print(f"All info from {sender}: {altitude}, {distance}, {status}")
        elif compBytesStr(data_bytes, "IR:R"):
            print(f"All info repeat received from {sender}")
        elif compBytesStr(data_bytes, "IR"):
            #all info but repeating
            altitude = int.from_bytes(data_bytes[2:6], byteorder='big')
            distance = int.from_bytes(data_bytes[6:8], byteorder='big')
            status = int.from_bytes(data_bytes[8:], byteorder='big')
            print(f"Repeating info from {sender}: {altitude}, {distance}, {status}")

        elif compBytesStr(data_bytes, "JFWD:R"):
            print(f"Jog Forward received from {sender}")
        elif compBytesStr(data_bytes, "JREV:R"):
            print(f"Jog Reverse received from {sender}")

@app.route('/close_serial/<device_name>')
def close_serial(device_name):
    try:
        if device_name == "payload":
            payloadSender.close()
        elif device_name == "recovery":
            recoverySender.close()

    except Exception as e:
        msg = f"Unable to Close Serial Connection: {e}"
        print(msg)
        return jsonify(message = msg), 500
    return jsonify(message='OK'),200

# Route for sending a command to a serial device
# The command will come as a json object with the following format:
# {
#   "device_name": "payload" or "recovery",
#   "destination_address": int,
#   "command": [{data, type}, {data, type}, ...]
# }
# The data will be of different types
@app.route('/send_command', methods=['POST'])
def send_command():
    content_type = request.headers.get('Content-Type')
    if (content_type == 'application/json'):
        json = request.json
        device_name = json['device_name']
        command = json['command']
        address = int(json['destination_address'])
        # The command is a bunch of data of different types
        # To bulid the command we need to convert the data to bytes
        # The types are: string, int, float, bool
        # The command will be sent as a byte array

        command_bytes = bytearray()
        for data in command:
            if data['type'] == 'string':
                command_bytes.extend(data['data'].encode('utf-8'))
            elif data['type'] == 'int':
                command_bytes.extend(data['data'].to_bytes(4, byteorder='big'))
            elif data['type'] == 'float':
                # convert the float to a byte array
                command_bytes.extend(bytearray(struct.pack(">f", data['data'])))
            elif data['type'] == 'bool':
                command_bytes.extend(data['data'].to_bytes(1, byteorder='big'))
        # will be in the format: {0x02}{0x02}{destination_address 2B}{message_length 2B}{data_bytes undefined length}\n
        final_command = bytearray()
        final_command.extend(b'\x02\x02')
        final_command.extend(address.to_bytes(2, byteorder='big'))
        final_command.extend(len(command_bytes).to_bytes(2, byteorder='big'))
        final_command.extend(command_bytes)
        final_command.extend(b'\n')
        
        if device_name == "payload":
            payloadSender.serial_input_bytes(final_command)
        elif device_name == "recovery":
            recoverySender.serial_input_bytes(final_command)
        return jsonify(message = "OK"), 200
    else:
        return 'Content-Type not supported!'