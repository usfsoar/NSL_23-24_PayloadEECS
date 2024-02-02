import sys
from mods.sensors import save_to_csv, create_new_csv
import mods.comms as comms
import mods.shout_it as shout_it
import mods.dr_love as dr_love
import argparse

import signal

# Made contribution to this file
print("Starting Zero")
# Yay! I'm a contributor

parser = argparse.ArgumentParser(description="Raspberry Pi Zero Procedure for SOAR NSL")
parser.add_argument("--dummy-lora", action='store_true',help='Use dummy data for LoRa')
parser.add_argument("--log-slave", action='store_true', help='Log output from slave readings')
parser.add_argument("--skip-bt", action='store_true', help='For debugging purposes skip bluetooth setup')
args = parser.parse_args()
DUMMY_LORA = args.dummy_lora
deployment_started = False

def signal_handler(sig,frame):
    print("Exit handler")
    sys.exit()
signal.signal(signal.SIGTERM, signal_handler)

try:
    dr_love.reset()
    create_new_csv()
    if not args.skip_bt:
        shout_it.connect_ble()
except Exception as e:
    print("Startup errors", e)

try:
    print("Starting PAYLOAD")
    while True:
        save_to_csv()
        status = comms.receiveMessage(args.log_slave)
        if status == "DEPLOY" or DUMMY_LORA:
            print("Triggering deployment")
            comms.sendAcknowledge("ATTEMPT DEPLOY")
            deployment_started = shout_it.send_deploy_with_res()
            if deployment_started:
                comms.sendAcknowledge("DEPLOY STARTED")
        elif status == "STOP":
            print("Stopping deployment")
            comms.sendAcknowledge("ATTEMPT STOP")
            deployment_stopped = shout_it.send_with_res("STOP","OK", timeout=3)
            if deployment_stopped:
                comms.sendAcknowledge("DEPLOY STOPING")
        elif status == "RESET":
            print("Resetting states")
            comms.sendAcknowledge("ATTEMPT RESET")
            deployment_resetting = shout_it.send_with_res("RESET", "OK", timeout=3)
            if deployment_resetting:
                comms.sendAcknowledge("STATES RESETING")
        elif status =="RETRACT":
            print("Retracting stepper")
            comms.sendAcknowledge("ATTEMPT RETRACTING")
            deployment_retracting = shout_it.send_with_res("RETRACT", "OK", timeout=3)
            if deployment_retracting:
                comms.sendAcknowledge("DEPLOY RETRACTING")
except Exception as e:
    print(f'Global error: {e}')