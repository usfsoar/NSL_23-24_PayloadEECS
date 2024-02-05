class IMU:
    def __init__(self, acceleration_x=None, acceleration_y=None, acceleration_z=None,
                 gyro_x=None, gyro_y=None, gyro_z=None, magnetic_x=None, magnetic_y=None, magnetic_z=None):
        self.acceleration_x = acceleration_x
        self.acceleration_y = acceleration_y
        self.acceleration_z = acceleration_z
        self.gyro_x = gyro_x
        self.gyro_y = gyro_y
        self.gyro_z = gyro_z
        self.magnetic_x = magnetic_x
        self.magnetic_y = magnetic_y
        self.magnetic_z = magnetic_z

# Example usage:
# imu_data = IMU(acceleration_x=0.1, acceleration_y=0.2, acceleration_z=9.8, gyro_x=1.5, gyro_y=2.0, gyro_z=0.5)
# imu_data.acceleration_x will give you the acceleration_x value
# imu_data.gyro_y will give you the gyro_y value
# Similarly for other properties

