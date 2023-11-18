import smbus2
import time
import csv
import board
import adafruit_bmp3xx
import adafruit_bno055
import adafruit_adxl34x

# I2C addresses for the sensors
bmp390_address = 0x76  # BMP390 I2C address
bno055_address = 0x28  # BNO055 I2C address
adxl345_address = 0x53 # ADXL345 I2C address
arduino_address=0x08

# Create an I2C bus
bus = smbus2.SMBus(1)  # 1 indicates /dev/i2c-1
i2c = board.I2C()

def read_bmp390():
    
    bmp = adafruit_bmp3xx.BMP3XX_I2C(i2c)
    bmp.sea_level_pressure = 1013.25

    return bmp.pressure, bmp.temperature, bmp.altitude

def read_bno055():
    bno = adafruit_bno055.BNO055_I2C(i2c)
    # Read the Euler angles for heading, roll, pitch (all in degrees).
    heading, roll, pitch = bno.read_euler()
    # Read the calibration status, 0=uncalibrated and 3=fully calibrated.
    sys, gyro, accel, mag = bno.get_calibration_status()
    # Orientation as a quaternion:
    x_qua,y_qua,z_qua,w_qua = bno.read_quaterion()
    # Sensor temperature in degrees Celsius:
    temp_c = bno.read_temp()
    # Magnetometer data (in micro-Teslas):
    x_mag,y_mag,z_mag = bno.read_magnetometer()
    # Gyroscope data (in degrees per second):
    x_gyro,y_gyro,z_gyro = bno.read_gyroscope()
    # Accelerometer data (in meters per second squared):
    x_acc,y_acc,z_acc = bno.read_accelerometer()
    # Linear acceleration data (i.e. acceleration from movement, not gravity--
    # returned in meters per second squared):
    x_lin,y_lin,z_lin = bno.read_linear_acceleration()
    # Gravity acceleration data (i.e. acceleration just from gravity--returned
    # in meters per second squared):
    x_grav,y_grav,z_grav = bno.read_gravity()
    
    return heading, roll, pitch, sys, gyro, accel, mag, x_qua, y_qua, z_qua, w_qua, temp_c, x_mag, y_mag, z_mag, x_gyro, y_gyro, z_gyro, x_acc, y_acc, z_acc, x_lin, y_lin, z_lin, x_grav, y_grav, z_grav

def read_adxl345():
    accelerometer = adafruit_adxl34x.ADXL345(i2c)
    x,y,z=accelerometer.acceleration

    return x, y, z

def save_to_csv():
    fields=['Pressure', 'Temperature', 'Altitude', 'Heading', 'Roll', 'Pitch', 'sys', 'gyro', 'accel', 'mag', 'x_qua', 'y_qua', 'z_qua', 'w_qua',
    'temp_c', 'x_mag', 'y_mag', 'z_mag', 'x_gyro', 'y_gyro', 'z_gyro', 'x_acc', 'y_acc', 'z_acc', 'x_lin', 'y_lin', 'z_lin', 'x_grav', 'y_grav', 'z_grav',
      'X_adx', 'Y_adx', 'Z_adx']
    with open("data.csv", "w") as output:
        csvwriter = csv.writer(output)
        csvwriter.writerow(fields)
        pressure, temperature, altitude = read_bmp390()
        heading, roll, pitch, sys, gyro, accel, mag, x_qua, y_qua, z_qua, w_qua, temp_c, x_mag, y_mag, z_mag, x_gyro, y_gyro, z_gyro, x_acc, y_acc, z_acc, x_lin, y_lin, z_lin, x_grav, y_grav, z_grav = read_bno055()
        x, y, z = read_adxl345()
        #save data to csv file
        data_row=[pressure, temperature, altitude, heading, roll, pitch, sys, gyro, accel, mag, x_qua, y_qua, z_qua, w_qua, temp_c, x_mag, y_mag, z_mag, x_gyro, y_gyro, z_gyro, x_acc, y_acc, z_acc, x_lin, y_lin, z_lin, x_grav, y_grav, z_grav, x, y, z]
        csvwriter.writerow(data_row)
        #send altitude value to arduino
        bus.write_i2c_block_data(arduino_address, 0, altitude)
        time.sleep(1)
    output.close()
    # print(f"BMP390 - Pressure: {pressure} hPa, Temperature: {temperature} °C")
    # print(f"BNO055 - Heading: {heading}°, Roll: {roll}°, Pitch: {pitch}°")
    # print(f"ADXL345 - Acceleration - X: {x}g, Y: {y}g, Z: {z}g")

if __name__=='__main__':
    save_to_csv()