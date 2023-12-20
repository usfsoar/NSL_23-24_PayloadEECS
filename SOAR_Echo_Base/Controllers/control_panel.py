import time
from Config import *
import Services.lora as lora
from flask import Flask, render_template, jsonify
from threading import Thread
import re
@app.route('/')
def control_panel():
    return render_template('control_panel.html')

#@app.route('/button_test')
#def button_test():
#    # Trigger deployment of the payload
#    # Sending an RF signal
#    lora.trigger_deploy()
#    return "Hello from server"

@socketio.on('disconnect')
def test_disconnect():
    print('Client disconnected')
    
@app.route('/random_cmd')
def random_cmd():
    # Trigger deployment of the payload
    # Sending an RF signa
    lora.send_random()
    return "Hello from server"


@app.route('/close_serial')
def close_serial():
    try:
        lora.close()
    except Exception as e:
        msg = f"Unable to Close Serial Connection: {e}"
        print(msg)
        return jsonify(message = msg), 500
    return jsonify(message='OK'),200

# SENDER FUNCTIONS
def update_alti(alti):
    socketio.emit('alti_update',{'alti':alti})

def log_msg(message):
    socketio.emit('alti_log', {'message':message})