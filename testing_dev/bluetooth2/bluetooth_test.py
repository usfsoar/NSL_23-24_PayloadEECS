import bluetooth
import time
hc06_address = "FC:A8:9A:00:22:AB"

sock = bluetooth.BluetoothSocket(bluetooth.RFCOMM)

port =1

try:
	sock.connect((hc06_address, port))
	print("Connected to HC-06")
	sock.send("DEPLOY")
	data = ""
	sock.send("DEPLOY")
	while True:
		char = sock.recv(1).decode()
		data += char
		if char == '\n':
			break
	print("Received:", data)
except bluetooth.BluetoothError as e:
	print("Error:", e)

except Exception as e:
	print("Error:", e)

finally:
	sock.close()
	print("Connection closed")
