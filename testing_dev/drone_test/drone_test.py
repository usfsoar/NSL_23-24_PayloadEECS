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

# Disarm the drone
def disarm_drone():
    master.arducopter_disarm()

# Define keyboard listener
def on_key_release(key):
    try:
        if key.char == 'a':
            arm_drone()
            print("Arming drone!")
        elif key.char == 'q':
            disarm_drone()
            print("Disarming drone!")
            exit()

    except Exception as e:
        print(f"Error: {e}")

# Start keyboard listener
with keyboard.Listener(on_release=on_key_release) as listener:
    listener.join()
