import re
from Controllers import gps_controller

def relay_message(message):
    if "GPS" in message:
        pattern = r'GPS:(.*?)<\/LORA>'
        match = re.search(pattern, message)
        if bool(match):
            nmea = match.group(1)
            gps_controller.update_gps(nmea)
            return
        #Relay the message as something to be logged
        gps_controller.log_msg(message)