from Server.Config import *
import RF_Service.lora as lora
from flask import Flask, render_template
from threading import Thread


@app.route('/gps_data')
def gps_controller():
    return render_template('gps_data.html')

@socketio.on('connect')
def test_connect():
    print('Client connected')

@socketio.on('disconnect')
def test_disconnect():
    print('Client disconnected')
    
def display_gps_data():
    #WIll receive data
    pass

@app.route('/button_test')
def gps_start():
    # start sending gps data

    gps_thread = Thread(target = lora.receive_data)
    gps_thread.daemon = True
    gps_thread.start()
    return ""