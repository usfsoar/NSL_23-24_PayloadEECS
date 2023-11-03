from flask import Flask, render_template
from flask_socketio import SocketIO
import sys
import threading
import socket
import logging

log = logging.getLogger('werkzeug')
log.setLevel(logging.ERROR)

app = Flask(__name__)
app.logger.setLevel(logging.ERROR)
socketio = SocketIO(app)

@app.route('/')
def index():
    return "Hello world!"

def run():
    try:
        socketio.run(app, host='0.0.0.0', port=5000)
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