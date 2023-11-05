from flask import Flask, render_template
from flask_socketio import SocketIO
import sys
import threading
import socket
import logging
import os

app = Flask(__name__)
socketio = SocketIO(app)

@app.route('/')
def index():
    return "Hello world!"

def run():
    try:
        socketio.run(app, host='0.0.0.0', allow_unsafe_werkzeug=True, port=os.getenv("PORT", default=5000))
    except KeyboardInterrupt:
        print('Shutting down server...')
        socketio.stop()
        #Exit the program
        sys.exit(0)

def get_ip_address():
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.connect(("8.8.8.8", 80))
    return s.getsockname()[0]

ip_address = get_ip_address()
port = 5000  
print(f"Access the site at: http://{ip_address}:{port}/")

server_thread = threading.Thread(target=run)
server_thread.daemon = True
server_thread.start()