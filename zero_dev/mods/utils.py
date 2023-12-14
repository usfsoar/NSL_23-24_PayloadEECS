from signal import *
import sys

def cleanup(*args):
    sys.exit(0)

def exitListen():
    try:
        for sig in (SIGABRT, SIGILL, SIGINT, SIGSEGV, SIGTERM):
            signal(sig, cleanup)
    except Exception as e:
        print(f'Error setting up signal handler: {e}')
        