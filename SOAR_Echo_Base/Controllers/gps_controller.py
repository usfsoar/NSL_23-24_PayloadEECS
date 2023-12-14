from Config import *
import Services.lora as lora
from flask import Flask, render_template, jsonify
from threading import Thread
import re


@app.route('/gps_data')
def gps_controller():
    return render_template('gps_data.html')

@socketio.on('connect')
def test_connect():
    print('Client connected')

@socketio.on('gps_request')
def gps_request():
    for i in lora.lora:
        if "GPS" in i:
            pattern = r'GPS:(.*?)<\/LORA>'
            nmea = re.match(pattern, i)
            if nmea:
                socketio.emit(nmea)
            else:
                print("GPS Failed to connnect")

@socketio.on('disconnect')
def test_disconnect():
    print('Client disconnected')
    
def display_gps_data():
    #WIll receive data
    pass

@app.route('/gps_start')
def gps_start():
    # start the gps thread
    try:
        lora.connect()
        gps_thread = Thread(target = lora.receive_data)
        gps_thread.daemon = True
        gps_thread.start()
    except Exception as e:
        msg = f"Exception with GPS system: {e}"
        print(msg)
        return jsonify(message = msg), 500
    return jsonify(message='OK'),200