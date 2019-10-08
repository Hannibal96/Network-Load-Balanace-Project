import matplotlib.pyplot as plt
import sys
import numpy as np
file_name = "./../results/Servers-10x0.01,10x0.1,5x1,-JSQ-Load-0.950000-Buffer-Low=0-High=0_values.log"
descriptor = file_name.split('/')[3].split('_')[0]
x = open(file_name)
time = list()
avg_serving = list()
buffer = list()
stable = list()

for idx, line in enumerate(x):
    splited = line.split()
    time.append(int(splited[0]))
    avg_serving.append(int(splited[1]))
    stable.append(int(splited[2]))
    buffer.append(int(splited[3]))

time = np.array(time)
stable = np.array(stable)
avg_serving = np.array(avg_serving)
buffer = np.array(buffer)

plt.plot(time, stable)
plt.yticks(np.arange(0, max(stable), max(stable)/20))
plt.title("Convergance\n"+descriptor)
plt.show()

plt.plot(time, buffer)
plt.yticks(np.arange(0, max(buffer), max(buffer)/20 + int(not(max(buffer)/20))))
plt.title("Buffer Size\n"+descriptor)
plt.show()

plt.plot(time, avg_serving)
plt.yticks(np.arange(0, max(avg_serving), max(avg_serving)/20))
plt.title("Avg Serving\n"+descriptor)
plt.show()
