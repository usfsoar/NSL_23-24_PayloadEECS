import os
import shutil
import sys

def copy_to_arduino_libraries():
    # Determine the Arduino libraries directory based on the platform
    if sys.platform.startswith('win'):
        arduino_libraries_dir = os.path.join(os.path.expanduser('~'), 'Documents', 'Arduino', 'libraries')
    elif sys.platform.startswith('darwin'):
        arduino_libraries_dir = os.path.join(os.path.expanduser('~'), 'Documents', 'Arduino', 'libraries')
    elif sys.platform.startswith('linux'):
        arduino_libraries_dir = os.path.join(os.path.expanduser('~'), 'Arduino', 'libraries')
    else:
        print("Unsupported platform.")
        return

    # Get the name of the current directory
    current_dir = os.path.basename(os.getcwd())

    # Destination directory for the library
    destination_dir = os.path.join(arduino_libraries_dir, current_dir)

    # Copy the current directory to the Arduino libraries directory
    try:
        if os.path.exists(destination_dir):
            shutil.rmtree(destination_dir)
        shutil.copytree(os.getcwd(), destination_dir, ignore=shutil.ignore_patterns('*.py'))
        print(f"Successfully copied {current_dir} to Arduino libraries directory.")
    except Exception as e:
        print(f"An error occurred: {str(e)}")

if __name__ == "__main__":
    copy_to_arduino_libraries()
