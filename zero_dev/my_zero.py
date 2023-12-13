import sys
from mods.sensors import save_to_csv, create_new_csv
import string
import mods.comms as comms
import mods.shout_it as shout_it
import argparse

parser = argparse.ArgumentParser(description="Raspberry Pi Zero Procedure for SOAR NSL")
parser.add_argument("--dummy-lora", action='store_true',help='Use dummy data for LoRa')
parser.add_argument("--log-slave", action='store_true', help='Log output from slave readings')
parser.add_argument("--skip-bluetooth", action='store_true', help='For debugging purposes skip bluetooth setup')
args = parser.parse_args()
DUMMY_LORA = args.dummy_lora
deployment_started = False


try:
    create_new_csv()
    if not args.skip_bluetooth:
        shout_it.connect_ble()
except Exception as e:
    print("Startup errors", e)
    print("Hopefully we can still proceed. I'm one with the force and the force is with me")

while True:
    try:   
        save_to_csv()
        # status = comms.receiveMessage(args.log_slave).strip("\x07 \t\r\n")
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
                comms.sendAcknowledge("DEPLOY STOPPED")
    except KeyboardInterrupt:
        print("\nQuitting") 
        sys.exit()
    except Exception as e:
        print(e)
