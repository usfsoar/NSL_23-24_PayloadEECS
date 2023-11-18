import bluetooth
import time

def bluetooth():
    
    hc06_address = "FC:A8:9A:00:22:AB"

    sock = bluetooth.BluetoothSocket(bluetooth.RFCOMM)

    port =1

    try:
        sock.connect((hc06_address, port))
        print("Connected to HC-06")
        sock.send("DEPLOY")
        data = ""
        start_time = time.time()
        while time.time()-start_time < 10:
            char = sock.recv(1).decode()
            if char == '\n':
                break
            data += char
        print("Received:", data)
        if(data is "OK"):
            sock.close()
            print("Connection closed")
            return True
        
    except bluetooth.BluetoothError as e:
        print("Error:", e)

    except Exception as e:
        print("Error:", e)

    finally:
        sock.close()
        print("Connection closed")
        return False
