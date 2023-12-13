import smbus
import time
from datetime import datetime
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
bus = smbus.SMBus(1)  # 1 indicates /dev/i2c-1
i2c = board.I2C()

def read_bmp390():
    
    bmp = adafruit_bmp3xx.BMP3XX_I2C(i2c)
    bmp.sea_level_pressure = 1013.25

    return bmp.pressure, bmp.temperature, bmp.altitude

def read_bno055():
    bno = adafruit_bno055.BNO055_I2C(i2c)
    # Read the Euler angles for heading, roll, pitch (all in degrees).
    euler= bno.euler
    heading = euler[0]
    roll = euler[1]
    pitch = euler[2]
    # Orientation as a quaternion:
    # Sensor temperature in degrees Celsius:
    x_qua, y_qua, z_qua, w_qua = bno.quaternion
    temp_c = bno.temperature
    # Magnetometer data (in micro-Teslas):
    x_mag,y_mag,z_mag = bno.magnetic
    # Gyroscope data (in degrees per second):
    x_gyro,y_gyro,z_gyro = bno.gyro
    # Accelerometer data (in meters per second squared):
    x_acc,y_acc,z_acc = bno.acceleration
    # Linear acceleration data (i.e. acceleration from movement, not gravity--
    # returned in meters per second squared):
    x_lin,y_lin,z_lin = bno.linear_acceleration
    # Gravity acceleration data (i.e. acceleration just from gravity--returned
    # in meters per second squared):
    x_grav,y_grav,z_grav = bno.gravity
    
    return heading, roll, pitch,x_qua, y_qua, z_qua, w_qua, temp_c, x_mag, y_mag, z_mag, x_gyro, y_gyro, z_gyro, x_acc, y_acc, z_acc, x_lin, y_lin, z_lin, x_grav, y_grav, z_grav

def read_adxl345():
    accelerometer = adafruit_adxl34x.ADXL345(i2c)
    x,y,z=accelerometer.acceleration

    return x, y, z

def create_new_csv():
    fields=['Time', 'Pressure', 'Temperature', 'Altitude', 'Heading', 'Roll', 'Pitch', 'x_qua', 'y_qua', 'z_qua', 'w_qua',
    'temp_c', 'x_mag', 'y_mag', 'z_mag', 'x_gyro', 'y_gyro', 'z_gyro', 'x_acc', 'y_acc', 'z_acc', 'x_lin', 'y_lin', 'z_lin', 'x_grav', 'y_grav', 'z_grav',
      'X_adx', 'Y_adx', 'Z_adx']
    with open("./data/data.csv", "w") as output:
        csvwriter = csv.writer(output)
        csvwriter.writerow(fields)
    output.close()

def save_to_csv():
    try:
        with open("./data/data.csv", "a") as output:
            csvwriter = csv.writer(output)
            timestamp=datetime.now()
            try:
                pressure, temperature, altitude = read_bmp390()
            except:
                pressure, temperature, altitude=0,0,0
            try:
                heading, roll, pitch, x_qua, y_qua, z_qua, w_qua, temp_c, x_mag, y_mag, z_mag, x_gyro, y_gyro, z_gyro, x_acc, y_acc, z_acc, x_lin, y_lin, z_lin, x_grav, y_grav, z_grav = read_bno055()
            except:
                heading, roll, pitch, x_qua, y_qua, z_qua, w_qua, temp_c, x_mag, y_mag, z_mag, x_gyro, y_gyro, z_gyro, x_acc, y_acc, z_acc, x_lin, y_lin, z_lin, x_grav, y_grav, z_grav=0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
            try:
                x, y, z = read_adxl345()
            except:
                x,y,z=0,0,0
            #save data to csv file

            data_row=[timestamp, pressure, temperature, altitude, heading, roll, pitch,  x_qua, y_qua, z_qua, w_qua, temp_c, x_mag, y_mag, z_mag, x_gyro, y_gyro, z_gyro, x_acc, y_acc, z_acc, x_lin, y_lin, z_lin, x_grav, y_grav, z_grav, x, y, z]
            try:
                csvwriter.writerow(data_row)
                #send altitude value to arduino
                msg = f'Alt:{altitude:.1f}'
                encoded_msg = [ord(c) for c in msg]
                bus.write_i2c_block_data(arduino_address, 0, encoded_msg)
            except Exception as e:
                print(f'Error sending data back:{e}')

            time.sleep(0.1)
        output.close()
        # print(f"BMP390 - Pressure: {pressure} hPa, Temperature: {temperature} °C")
        # print(f"BNO055 - Heading: {heading}°, Roll: {roll}°, Pitch: {pitch}°")
        # print(f"ADXL345 - Acceleration - X: {x}g, Y: {y}g, Z: {z}g")
        return True
    except Exception as e:
        print(f'Excpetion reading sensor data: {e}')
        return False

if __name__=='__main__':
    i = 1
    create_new_csv()
    while(i<10):
        save_to_csv()
        i+=1
    