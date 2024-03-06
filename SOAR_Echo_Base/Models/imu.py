class IMU:
    def __init__(
        self,
        euler_vector=None,
        quaternion=None,
        angular_velocity=None,
        acceleration=None,
        magnetic_field=None,
        linear_acceleration=None,
        gravity_vector=None,
        temperature=None,
    ):
        """
        Initialize the IMU object. All parameters are optional.

        Args:
            euler_vector (list[float], optional): The Euler angles [roll, pitch, yaw].
            quaternion (list[float], optional): [w, x, y, z].
            angular_velocity (list[float], optional): [x, y, z].
            acceleration (list[float], optional): [x, y, z].
            magnetic_field (list[float], optional: [x, y, z].
            linear_acceleration (list[float], optional): [x, y, z].
            gravity_vector (list[float], optional): [x, y, z].
            temperature (float, optional): The temperature in degrees Celsius.
        """
        self.euler_vector = euler_vector
        self.quaternion = quaternion
        self.angular_velocity = angular_velocity
        self.acceleration = acceleration
        self.magnetic_field = magnetic_field
        self.linear_acceleration = linear_acceleration
        self.gravity_vector = gravity_vector
        self.temperature = temperature

# Example usage:
# imu_data = IMU(quaternion=[0.5, 0.5, 0.5, 0.5], angular_velocity=[1.0, 2.0, 3.0])
# imu_data.angular_velocity[1] will give you the y value
# imu_data.quaternion[0] will give you the w value
# Similarly for other properties

