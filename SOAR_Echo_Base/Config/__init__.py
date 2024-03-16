from Services.ip_port import get_port, display_ip_address
from flask import Flask, render_template, send_from_directory, request
from flask_socketio import SocketIO
import sys
import os


app = Flask(__name__, template_folder = '../Templates', static_folder = '../static')
socketio = SocketIO(app)

@socketio.on('connect')
def socket_connect():
    print("Client connected")

@app.route('/<path:filename>')
def custom_static(filename):
    # Fallback to a default directory if no referrer is found
    # filename = filename.replace('@', '%40')
    directory = os.path.join(app.root_path,'..', 'Views')
    # Check if the directory with the filename exists, just print exists
    if os.path.exists(os.path.join(directory, filename)):
        print(f"EXISTS: {os.path.join(directory, filename)}")
    else:
        print(f"DOES NOT EXIST: {os.path.join(directory, filename)}")
    return send_from_directory(directory, filename)

def run_server():
    display_ip_address()
    try:
        socketio.run(app, host='0.0.0.0', allow_unsafe_werkzeug=True, port=get_port(), debug=True)
    except KeyboardInterrupt:
        print('Shutting down server...')
        socketio.stop()
        #Exit the program
        sys.exit(0)

