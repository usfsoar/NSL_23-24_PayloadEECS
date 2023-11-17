from Server.Config import *
import RF_Service.lora as lora
from flask import Flask, render_template

@app.route('/')
def control_panel():
    return render_template('control_panel.html')

#@app.route('/button_test')
#def button_test():
#    # Trigger deployment of the payload
#    # Sending an RF signal
#    lora.trigger_deploy()
#    return "Hello from server"
@app.route('/random_cmd')
def random_cmd():
    # Trigger deployment of the payload
    # Sending an RF signal
    lora.send_random()
    return "Hello from server"
