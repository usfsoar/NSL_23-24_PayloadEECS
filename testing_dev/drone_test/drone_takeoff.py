from pymavlink import mavutil
from pynput import keyboard

master = mavutil.mavlink_connection("COM22")

master.wait_heartbeat()

print(f'Heartbeat from system {master.target_system} component {master.target_component}')

master.mav.command_long_send(master.target_system, master.target_component,
                         mavutil.mavlink.MAV_CMD_COMPONENT_ARM_DISARM , 0, 1, 0, 0, 0, 0, 0, 0)

msg = master.recv_match(type='COMMAND_ACK', blocking=True)
print(msg)
# Takeoff
master.mav.command_long_send(master.target_system, master.target_component,
                             mavutil.mavlink.MAV_CMD_NAV_TAKEOFF, 0, 0, 0, 0, 0, 0, 0, 5)
msg = master.recv_match(type='COMMAND_ACK', blocking=True)
print(msg)

# Disarm the drone
def disarm_drone():
    master.mav.command_long_send(master.target_system, master.target_component,
                                mavutil.mavlink.MAV_CMD_COMPONENT_ARM_DISARM , 0, 0, 21196, 0, 0, 0, 0, 0)
    msg = master.recv_match(type='COMMAND_ACK', blocking=True)
    print(msg)

def on_key_release(key):
    try:
        if key.char == 'q':
            disarm_drone()
            print("Disarming drone!")
            exit()

    except Exception as e:
        print(f"Error: {e}")

# Start keyboard listener
with keyboard.Listener(on_release=on_key_release) as listener:
    listener.join()