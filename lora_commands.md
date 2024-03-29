## Drone

## Deployment
|Command|Definition|Response|Definition|
|---|---|---|---|
|`PI`|Ping|`PO{T-time}`|Pong with time|
|`AS`|Altitude Single|`AS{T-time}{Altitude-float}`|Altitude Single Data with time|
|`AR`|Altitude Repeat|`AR{T-time}`|Altitude Repeat Received with time|
|`AW`|Write altitude thresholds|`AW{T-time}`|Thresholds written with time|
|||`AF{T-time}`|Failed to set thresholds with time|
|`AT`|Get altitude thresholds|`AT{T-time}{H1-float}{H2-float}{H3-float}`|Thresholds data with time|
|`DP`|Deploy|`DP{T-time}`|Deploy Received with time|
|`DS`|Deploy Status|`DS{T-time}{Status-uint8_t}`|Deploy Status Data with time|
|`DR`|Deploy Status Repeat|`DR{T-time}`|Deploy Status Repeat Received with time|
|`DT`|Deploy Stop|`DT{T-time}`|Deploy Stop Received with time|
|`DR`|Deploy Reset|`DR{T-time}`|Deploy Reset Received with time|
|`DC`|Deploy Retract|`DC{T-time}``|Deploy Retract Received with time|
|`LI`|Distance sensor|`LI{T-time}{Distance-uint16_t}`|Distance sensor data with time|
|`LR`|Distance sensor repeat|`LR{T-time}`|Distance sensor repeat received with time|
|`IS`|All info single|`IS{T-time}{Altitude-float}{Distance-uint16_t}{Status-uint8_t}`|All info data with time|
|`IR`|All info repeat|`IR{T-time}`|All info repeat received with time|
|`JF`|Jog Forward|`JF{T-time}`|Jog Forward Received with time|
|`JR`|Jog Reverse|`JR{T-time}`|Jog Reverse Received with time|
|`RS`|Stop any repeating data|`RS{T-time}`|Stop any repeating data received with time|
|`NH`|Not handled (n bytes)|`NH{T-time}{Command-nB}`|Not handled command with time|


## Recovery
|Command|Definition|Response|Definition|
|---|---|---|---|
|`PI`|Ping|`PO`|Pong|
|`GS`|GPS Single|`GS:R`| GPS Single Received|
|||`GS:{NMEA String}`|GPS NEMA reponse|
|||`GS:F`|GPS Failed to get a fix|
|`GR`|GPS Repeat|`GR:R`|GPS Repeat Received|
|||`GR:{NMEA String}`|GPS NEMA reponse|
|||`GR:F`|GPS Failed to get a fix|
|`IM`|IMU Single|`IM:R`|IMU Single Received|
|||`IM{AccelX-4B}{AccelY-4B}{AccelZ-4B}{GyroX-4B}{GyroY-4B}{GyroZ-4B}`|IMU Data (14 bytes total)|
|`IR`|IMU Repeat|`IR:R`|IMU Repeat Received|
|||`IR{AccelX-4B}{AccelY-4B}{AccelZ-4B}{GyroX-4B}{GyroY-4B}{GyroZ-4B}`|IMU Data (14 bytes total)|


## LoRa Telemetry Commands
|Command|Definition|Response|Definition|
|---|---|---|---|
|`PI`|Ping|`PO{T-time}`|Pong with time|
|`AB`|Activate Parachute|`AB{T-time}`|Parachute Activated with time|
|`JL`|Jettison Lock|`JL{T-time}`|Jettison Locked with time|
|`JT`|Jettison Trigger|`JT{T-time}`|Jettison Triggered with time|
|`SS`|Send Sensor Data|`SS{T-time}{AccelX-float}{AccelY-float}{AccelZ-float}{GyroX-float}{GyroY-float}{GyroZ-float}{Temp-float}{Sound-float}{Pressure-float}`|Sensor Data with time|
|`SR`|Stemnaut Repeat|`SR{T-time}`|Repeat Stemnaut Data with time|
|`GS`|GPS Data Send|`GS{T-time}`|GPS Data Sent with time|
|`GR`|GPS Repeat|`GR{T-time}`|Repeat GPS Data with time|
|`NH`|Not Handled|`NH{T-time}{Command-nB}`|Not handled command with time|
