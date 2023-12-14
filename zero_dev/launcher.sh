#!/bin/sh
# launcher.sh
# Echo date and time
echo "Launching script at $(date)"

# Navigate to the script directory
cd /home/soar/NSL_23_24_PAYLOADEECS/zero_dev

# Save the current PID to a text file
echo $$ > my_zero_pid.txt

# Execute the Python script
# sudo -E python my_zero.py

# Change back to the home directory or any other desired location
cd /
