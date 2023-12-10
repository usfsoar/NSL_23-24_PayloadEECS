import sys
import threading
from Config import *
from Controllers import control_panel, gps_controller
import sys

if __name__=="__main__":
    try:
        # Server setup
        server_thread = threading.Thread(target=run_server)
        server_thread.daemon = True
        server_thread.start()
        
        # Other processes
        while True:
            pass
    except KeyboardInterrupt:
        print("Ctrl+C pressed shutting down")
        sys.exit()
    except Exception as e:
        print(f'Exception: {e}')