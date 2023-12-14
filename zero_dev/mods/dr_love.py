import RPi.GPIO as io
import time
try:
    resetPin = 27
    io.setmode(io.BCM)
    io.setup(resetPin, io.OUT)
    io.output(resetPin, True)
except Exception as e:
    print(f'Error setting up reset pin: {e}')

def reset():
    io.output(resetPin, False)
    time.sleep(0.5)
    io.output(resetPin, True)
    time.sleep(3)

if __name__=="__main__":
    reset()