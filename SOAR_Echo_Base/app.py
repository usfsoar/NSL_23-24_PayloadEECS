import Server.server as server
import sys
if __name__=="__main__":
    try:
        while True:
            pass
    except KeyboardInterrupt:
        print("Ctrl+C pressed shutting down")
        sys.exit()
    except Exception as e:
        print(f'Exception: {e}')