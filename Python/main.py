import matplotlib.pyplot as plt
import sys
import numpy as np
short_file_name = "Servers-10x0.1,10x0.2,5x0.5,-POC_2-Load-{0.5,1.0,0.025}-Buffer-Low=0-High=0-Time-1000000_values"
file_name = "./../results/"+short_file_name+".log"
descriptor = file_name.split('/')[3].split('_')[0]
x = open(file_name)
time = list()
avg_serving = list()
buffer = list()
stable = list()

for idx, line in enumerate(x):
    splited = line.split()
    time.append(float(splited[0]))
    avg_serving.append(float(splited[1]))
    stable.append(float(splited[2]))
    buffer.append(int(splited[3]))

time = np.array(time)
stable = np.array(stable)
avg_serving = np.array(avg_serving)
buffer = np.array(buffer)

plt.plot(time, stable)
plt.yticks(np.arange(0, max(stable), max(stable)/20))
plt.title("Convergance\n"+descriptor)
plt.savefig("./../results/"+descriptor+"_Convergance.png")
plt.show()

plt.plot(time, buffer)
plt.yticks(np.arange(0, max(buffer), max(buffer)/20 + int(not(max(buffer)/20))))
plt.title("Buffer Size\n"+descriptor)
plt.savefig("./../results/"+descriptor+"_Buffer.png")
plt.show()

plt.plot(time, avg_serving)
plt.yticks(np.arange(0, max(avg_serving), max(avg_serving)/20))
plt.title("Avg Serving\n"+descriptor)
plt.savefig("./../results/"+descriptor+"_Serving.png")
plt.show()
