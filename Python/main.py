import matplotlib.pyplot as plt
import os
import sys
import numpy as np

#short_file_name = "Servers-10x0.1,10x0.2,5x0.5,-POC-2-Load-{0.5,1.0,0.025}-Buffer-Low=0-High=0-Time-1000000_values"
#file_name = "./../results/"+short_file_name+".log"

files = os.listdir("./../results/")

for file_name in files:
    if not file_name.endswith("_values.log"):
        continue

    if (file_name[:-11]+"_Convergance.png") in files:
        continue

    short_file_name = file_name

    file_name = "./../results/"+short_file_name
    descriptor = file_name.split('/')[3].split('_')[0]
    plt_title = descriptor.split('}')[0]+"}\n"+descriptor.split('}')[1]
    x = open(file_name)
    time = list()
    avg_serving = list()
    buffer = list()
    stable = list()

    print(file_name)

    for idx, line in enumerate(x):
        splited = line.split()
        time.append(float(splited[0]))
        avg_serving.append(float(splited[1]))
        stable.append(float(splited[2]))
        buffer.append(int(splited[3]))

    if len(time) == 0:
        continue

    time = np.array(time)
    stable = np.array(stable)
    avg_serving = np.array(avg_serving)
    buffer = np.array(buffer)

    plt.plot(time, stable)
    plt.yticks(np.arange(0, max(stable), max(stable)/20))
    plt.ylabel('Convergance')
    if len(time) > 1000:
        plt.xlabel('Time')
    else:
        plt.xlabel('Load')
    plt.title("Convergance\n"+plt_title)
    plt.savefig("./../results/"+descriptor+"_Convergance.png")
    plt.show()

    plt.plot(time, buffer)
    plt.yticks(np.arange(0, max(buffer), max(buffer)/20 + int(not(max(buffer)/20))))
    plt.title("Buffer Size\n"+plt_title)
    plt.ylabel('Buffer size')
    if len(time) > 1000:
        plt.xlabel('Time')
    else:
        plt.xlabel('Load')
    plt.savefig("./../results/"+descriptor+"_Buffer.png")
    plt.show()

    plt.plot(time, avg_serving)
    plt.yticks(np.arange(0, max(avg_serving), max(avg_serving)/20))
    plt.ylabel('Average serving')
    if len(time) > 1000:
        plt.xlabel('Time')
    else:
        plt.xlabel('Load')
    plt.title("Avg Serving\n"+plt_title)
    plt.savefig("./../results/"+descriptor+"_Serving.png")
    plt.show()
