from Server.Config import *
import RF_Service.lora as lora
from flask import Flask, render_template


@app.route('/gps_data')
def gps_controller():
    return render_template('gps_data.html')