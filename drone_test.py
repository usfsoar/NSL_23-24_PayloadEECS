# Import necessary libraries
import time
from pynput import keyboard
from pymavlink import mavutil

# Set up MAVLink connection (adjust the connection string as needed)
connection_string = "COM22"  # Example: USB connection
master = mavutil.mavlink_connection(connection_string)

# Arm the drone
def arm_drone():
    master.arducopter_arm()

# Define keyboard listener
def on_key_release(key):
    try:
        if key == keyboard.Key.up:
            # Send MAVLink command to increase altitude
            master.mav.command_long_send(
                master.target_system,
                master.target_component,
                mavutil.mavlink.MAV_CMD_NAV_TAKEOFF,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
            )
            print("Going up!")
        elif key == keyboard.Key.down:
            # Send MAVLink command to decrease altitude
            master.mav.command_long_send(
                master.target_system,
                master.target_component,
                mavutil.mavlink.MAV_CMD_NAV_LAND,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
            )
            print("Going down!")
        elif key == keyboard.Key.space:
            # Send MAVLink command to maintain altitude (throttle input)
            master.mav.command_long_send(
                master.target_system,
                master.target_component,
                mavutil.mavlink.MAV_CMD_NAV_CONTINUE_AND_CHANGE_ALT,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
            )
            print("Maintaining altitude (throttle input)")

    except Exception as e:
        print(f"Error: {e}")

# Arm the drone before starting the listener
arm_drone()

# Start keyboard listener
with keyboard.Listener(on_release=on_key_release) as listener:
    listener.join()
