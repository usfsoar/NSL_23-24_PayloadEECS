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
                    # This nmea sentence is for the rocket
                    section = False
                else:
                    # This nmea sentence is for the payload
                    section = True
                gps_controller.update_gps(section, nmea)
            # Relay the message as something to be logged
            else:
                gps_controller.log_msg(message)
        elif "ALTITUDE" in message:  # Changed from "ALTI" to "ALTITUDE"
            control_panel.update_alti(
                message
            )  # Changed from update_alti to update_altitude
            control_panel.log_msg(message)
            print("Received altitude message")
        elif "IMU" in message:
            control_panel.update_imu(message)
            control_panel.log_msg(message)
            print("Received IMU message")
        elif "BAROMETER" in message:
            control_panel.update_barometer(message)
            control_panel.log_msg(message)
            print("Received barometer message")
        elif "DECIBEL" in message:
            control_panel.update_decibel(message)
            control_panel.log_msg(message)
            print("Received decibel message")
    except Exception as e:
        print(f"Error relaying serial message: {e}")
