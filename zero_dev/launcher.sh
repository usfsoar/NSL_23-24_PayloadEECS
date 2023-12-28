#!/bin/sh
# launcher.sh
# Echo date and time
timestamp=$(date +"%Y%m%d%H%M%S")
log_file="my_zero_log_$timestamp.txt"

echo "Launching script at $(date)"

# Navigate to the script directory
cd /home/soar/NSL_23_24_PAYLOADEECS/zero_dev

# Save the current PID to a text file
echo $$ > my_zero_pid.txt

# Execute the Python script and append the output to the log file
sudo -E python my_zero.py --log-slave >> "$log_file" 2>&1

# Change back to the home directory or any other desired location
cd /