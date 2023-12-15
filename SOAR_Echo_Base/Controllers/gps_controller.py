import time
from Config import *
import Services.lora as lora
from flask import Flask, render_template, jsonify
from threading import Thread
import re

# RECEIVER FUNCTIONS
@app.route('/gps_data')
def gps_controller():
    return render_template('gps_data.html')

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
        time.sleep(2)
        lora.gps_repeat()
    except Exception as e:
        msg = f"Exception with GPS system: {e}"
        print(msg)
        return jsonify(message = msg), 500
    return jsonify(message='OK'),200

# SENDER FUNCTIONS
def update_gps(nmea):
    socketio.emit('gps_update',{nmea:nmea})

def log_message(message):
    socketio.emig('gps_log', {message:message})