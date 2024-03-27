import pandas as pd
from matplotlib import pyplot as plt


plt.rcParams["figure.figsize"] = [2269.0,23.0]
plt.rcParams["figure.autolayout"] = True

##columns = ['time',"velocity_x","velocity_y","velocity_z","altitude"]

df = pd.read_csv("C:/Users/shani/Documents/NSL_23-24_PayloadEECS/drone_master/Drone_data.csv")
df['time'] = df['time']/1000

df.plot(x='time', y='velocity_x', label='velocity_x')
df.set_xlabel('time (seconds)')
df.set_ylabel('velocity x')
df.legend()

print("Contents in csv file:", df)

plt.show()

