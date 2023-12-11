from Utils.ip_port import get_port, display_ip_address

from flask import Flask, render_template
from flask_socketio import SocketIO
import sys


app = Flask(__name__, template_folder = '../Templates', static_folder = '../static')
socketio = SocketIO(app)

def run_server():
    display_ip_address()
    try:
        socketio.run(app, host='0.0.0.0', allow_unsafe_werkzeug=True, port=get_port(), debug=True)
    except KeyboardInterrupt:
        print('Shutting down server...')
        socketio.stop()
        #Exit the program
        sys.exit(0)
