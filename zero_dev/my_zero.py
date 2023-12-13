import sys
from mods.sensors import save_to_csv, create_new_csv
from mods.comms import receiveMessage
import mods.shout_it as shout_it
import argparse

parser = argparse.ArgumentParser(description="Raspberry Pi Zero Procedure for SOAR NSL")
parser.add_argument("--dummy_lora", action='store_true',help='Use dummy data for LoRa')
args = parser.parse_args()
DUMMY_LORA = args.dummy_lora
deployment_started = False


try:
    create_new_csv()
    shout_it.connect_ble()
except Exception as e:
    print("Startup errors", e)
    print("Hopefully we can still proceed. I'm one with the force and the force is with me")

while True:
    try:   
        save_to_csv()
        trigger_deploy = receiveMessage() == "DEPLOY" or DUMMY_LORA
        if trigger_deploy and not deployment_started:
            deployment_started = shout_it.send_deploy_with_res()
    except KeyboardInterrupt:
        print("\nQuitting") 
        sys.exit()
    except Exception as e:
        print(e)
