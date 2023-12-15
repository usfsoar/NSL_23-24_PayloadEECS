import re
from Controllers import gps_controller

def relay_message(message):
    try:
        if "GPS" in message:
            pattern = r'GPS:(.*?)<\/LORA>'
            match = re.search(pattern, message)
            if bool(match):
                nmea = match.group(1)
                gps_controller.update_gps(nmea)
                return
            #Relay the message as something to be logged
            gps_controller.log_msg(message)
    except Exception as e:
        print(f'Error relaying serial message: {e}')