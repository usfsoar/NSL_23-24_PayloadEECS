from Server.Config import *
from Server.Controller import hello, control_panel, gps_controller

import sys
import threading


server_thread = threading.Thread(target=run_server)
server_thread.daemon = True
server_thread.start()