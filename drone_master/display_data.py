import pandas as pd
from matplotlib import pyplot as plt

plt.rcParams["figure.figsize"] = [2269.0, 23.0]
plt.rcParams["figure.autolayout"] = True

df = pd.read_csv("Drone_data.csv")
df['time'] = df['time'] / 1000

# Strip spaces from column names
# df.columns = df.columns.str.strip()

# Plot the 'velocity_x' column
df.plot(x='time', y=' velocity_x', label='velocity_x')

# Set labels and legend
plt.xlabel('time (seconds)')
plt.ylabel('velocity x')
plt.legend()

print("Contents in csv file:", df)

plt.show()
