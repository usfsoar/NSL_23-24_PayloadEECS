import pandas as pd
import numpy as np
from matplotlib import pyplot as plt

plt.rcParams["figure.figsize"] = [2269.0, 23.0]
plt.rcParams["figure.autolayout"] = True

# Read the CSV file
df = pd.read_csv("Drone_data.csv")
df['time'] = df['time'] / 1000

# Strip spaces from column names
df.columns = df.columns.str.strip()

# Calculate the magnitude of acceleration
df['linacc_magnitude'] = np.sqrt(df['linacc_x']**2 + df['linacc_y']**2 + df['linacc_z']**2)

# Calculate velocity from acceleration
dt = np.gradient(df['time'])
df['velocity_x'] = np.cumsum(df['linacc_x'] * dt)
df['velocity_y'] = np.cumsum(df['linacc_y'] * dt)
df['velocity_z'] = np.cumsum(df['linacc_z'] * dt)
df['velocity_magnitude'] = np.sqrt(df['velocity_x']**2 + df['velocity_y']**2 + df['velocity_z']**2)

# Create a figure with 3 subplots
fig, axs = plt.subplots(3)

# Plot velocity components (x, y, and z) and their magnitude
df.plot(x='time', y=['velocity_x', 'velocity_y', 'velocity_z', 'velocity_magnitude'], ax=axs[0])
axs[0].set_ylabel('Velocity')
axs[0].legend()

# Plot altitude
df.plot(x='time', y='altitude', ax=axs[1])
axs[1].set_ylabel('Altitude')
axs[1].legend()

# Plot acceleration components (x, y, and z) and their magnitude
df.plot(x='time', y=['linacc_x', 'linacc_y', 'linacc_z', 'linacc_magnitude'], ax=axs[2])
axs[2].set_ylabel('Acceleration')
axs[2].legend()

# Set x label for the last subplot
axs[2].set_xlabel('Time (seconds)')

print("Contents in csv file:", df)

plt.show()
