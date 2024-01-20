import re
from Controllers import gps_controller, control_panel


def relay_message(message):
    try:
        pattern = r"<LORA>(.*?)<\/LORA>"
        match = re.search(pattern, message)
        if bool(match):
            control_panel.log_lora(match.group(1))
        if "GPS" in message:
            pattern = r"GPS:(.*?)<\/LORA>"
            match = re.search(pattern, message)
            if bool(match):
                nmea = match.group(1)
                # Check to see if this is the Rocket or the Payload
                if "RCKT" in message:
                    section = False
                else:
                    section = True
                gps_controller.update_gps(section, nmea)
            # Relay the message as something to be logged
            else:
                gps_controller.log_msg(message)
        elif "ALTI" in message:
            control_panel.update_alti(message)
            control_panel.log_msg(message)
    except Exception as e:
        print(f"Error relaying serial message: {e}")
