from mods.sensors import save_to_csv, create_new_csv
from mods.comms import receiveMessage
from mods.bluetooth import bluetooth

create_new_csv()
while True:
    try:   
        save_to_csv()
        if(receiveMessage()=="DEPLOY"):
            bluetooth()
        
    
    
    
    
    except Exception as e:
        print(e)
