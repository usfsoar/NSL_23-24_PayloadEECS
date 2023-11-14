from Server.Config import *
import RF_Service.lora as lora
from flask import Flask, render_template


@app.route('/gps_data')
def gps_controller():
    return render_template('gps_data.html')

@socketio.on('connect')
def test_connect():
    print('Client connected')

@socketio.on('disconnect')
def test_disconnect():
    print('Client disconnected')