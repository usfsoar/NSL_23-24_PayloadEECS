class Barometer:
    def __init__(self, pressure=None, temperature=None, altitude=None):
        self.pressure = pressure
        self.temperature = temperature
        self.altitude = altitude

# Example usage:
# barometer_data = Barometer(pressure=1013.25, temperature=25.5, altitude=100.0)
# barometer_data.pressure will give you the pressure value
# barometer_data.temperature will give you the temperature value
# barometer_data.altitude will give you the altitude value
