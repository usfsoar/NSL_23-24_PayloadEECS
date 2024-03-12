class IMU:
    def __init__(
        self,
        temperature=None,
        acceleration=None,
        magnetic=None,
        gyro=None,
        euler=None,
        quaternion=None,
        linear_acceleration=None,
        gravity=None,
        
    ):
        """
        Stores IMU sensor data. All parameters are optional.

        Args:
            temperature (float, optional).
            acceleration (list[float], optional): [x, y, z].
            magnetic (list[float], optional: Magnetometer values [x, y, z].
            gyro (list[float], optional): gyroscope values [x, y, z].
            euler (list[float], optional): The Euler angles [roll, pitch, yaw].
            quaternion (list[float], optional): [w, x, y, z].
            linear_acceleration (list[float], optional): linear acceleration values (without the effect of gravity) [x, y, z].
            gravity (list[float], optional): gravity acceleration (without the effect of linear acceleration) [x, y, z].
        """
        self.temperature = temperature
        self.acceleration = acceleration
        self.magnetic = magnetic
        self.gyro = gyro
        self.euler = euler
        self.quaternion = quaternion
        self.linear_acceleration = linear_acceleration
        self.gravity = gravity

# Example usage:
# imu_data = IMU(quaternion=[0.5, 0.5, 0.5, 0.5], acceleration=[1.0, 2.0, 3.0])
# imu_data.acceleration[1] will give you the y value
# imu_data.quaternion[0] will give you the w value
# Similarly for other properties

