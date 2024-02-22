import time
from Config import *
import Services.lora as lora
from flask import Flask, render_template, jsonify
from threading import Thread
import re


# RECEIVER FUNCTIONS
@app.route("/gps_data")
def gps_controller():
    return render_template("gps_data.html")


@socketio.on("disconnect")
def test_disconnect():
    print("Client disconnected")


def display_gps_data():
    # WIll receive data
    pass


@app.route("/telemetry_start/<port_name>")
def telemetry_start(port_name="COM7"):
    # start the gps/telemetry thread
    try:
        lora.connect(port_name)
        gps_telem_thread = Thread(target=lora.receive_data)
        gps_telem_thread.daemon = True
        gps_telem_thread.start()
        time.sleep(2)
        lora.gps_repeat()
        lora.telemetry_repeat()
    except Exception as e:
        msg = f"Exception with GPS/Telemetry system: {e}"
        print(msg)
        return jsonify(message=msg), 500
    return jsonify(message="OK"), 200


# @app.route('/close_serial_gps')
# def close_serial_gps():
#     try:
#         lora.close()
#     except Exception as e:
#         msg = f"Unable to Close Serial Connection: {e}"
#         print(msg)
#         return jsonify(message = msg), 500
#     return jsonify(message='OK'),200
# SENDER FUNCTIONS
def update_gps(section, nmea):
    socketio.emit("gps_update", {"section": section, "nmea": nmea})


def log_msg(message):
    socketio.emit("gps_log", {"message": message})
