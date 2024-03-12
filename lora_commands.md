## Drone

## Deployment
|Command|Definition|Response|Definition|
|---|---|---|---|
|`PI`|Ping|`PO`|Pong|
|`AS`|Altitude Single|`AS{Altitude-4B}`|Altitude Single Data|
|`AR`|Altitude Repeat|`AR:R`|Altitude Repeat Received|
|||`AR{Altitude-4B}`|Altitude Repeating Data|
|`AWTR{H1-4B}{H2-4B}{H3-4B}`|Write altitude thresholds|`AWTR:R`|Thresholds written|
|||`TW:F`|Failed to set thresholds|
|`AT`|Get altitude thresholds|`AT{H1-4B}{H2-4B}{H3-4B}`|Thresholds data|
|`DPLY`|Deploy|`DPLY:R`|Deploy Received|
|`DS`|Deploy Status|`DS{Status-2B}`|Deploy Status Data|
|`DSR`|Deploy Status Repeat|`DSR:R`|Deploy Status Repeat Received|
|||`DSR{Status-2B}`|Deploy Status Repeating Data|
|`DSTP`|Deploy Stop|`DSTP:R`|Deploy Stop Received|
|`DRST`|Deploy Reset|`DRST:R`|Deploy Reset Received|
|`DRTC`|Deploy Retract|`DRTC:R`|Deploy Retract Received|
|***Unprompted***|---|`DS{Status-2B}`|Deploy Status Data|
|`LI`|Distance sensor|`LI{Distance-2B}`|Distance sensor data|
|`LR`|Distance sensor repeat|`LR:R`|Distance sensor repeat received|
|||`LR{Distance-2B}`|Distance sensor data|
|`IS`|All info single|`IS{Altitude-4B}{Distance-2B}{Status-2B}`|All info data|
|`IR`|All info repeat|`IR:R`|All info repeat received|
|||`SR{Altitude-4B}{Distance-2B}{Status-2B}`|All info data|
|`JFWD`|Jog Forward|`JFWD:R`|Jog Forward Received|
|`JREV`|Jog Reverse|`JREV:R`|Jog Reverse Received|
|`RS`|Stop any repeating data|`RS:R`|Stop any repeating data received|
|`{random command}`|Not handled (n bytes)| `NH{Command-nB}`|Not handled command|

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

