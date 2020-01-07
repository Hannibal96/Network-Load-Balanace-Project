import matplotlib.pyplot as plt
import os
import numpy as np


def plot_vals(x_axis, y_axis, name, Descriptor):
    plt.plot(x_axis, y_axis)
    plt.yticks(np.arange(0, max(y_axis), max(y_axis) / 20))
    plt.ylabel(name)
    if len(x_axis) > 1000:
        plt.xlabel('Time')
    else:
        plt.xlabel('Load')
    plt.title(name+"\n" + plt_title)
    plt.grid()
    plt.savefig("./../results/" + Descriptor + "_" + name + ".png")
    plt.show()

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
    server_0 = list()
    server_1 = list()
    server_2 = list()
    server_3 = list()
    server_4 = list()
    print(file_name)

    for idx, line in enumerate(x):
        splited = line.split()
        time.append(float(splited[0]))
        avg_serving.append(float(splited[1]))
        stable.append(float(splited[2]))
        buffer.append(float(splited[3]))
        """
        server_0.append(float(splited[4]))
        server_1.append(float(splited[5]))
        server_2.append(float(splited[6]))
        server_3.append(float(splited[7]))
        server_4.append(float(splited[8]))
        """

    if len(time) == 0:
        continue

    time = np.array(time)
    stable = np.array(stable)
    avg_serving = np.array(avg_serving)
    buffer = np.array(buffer)
    server_0 = np.array(server_0)
    server_1 = np.array(server_1)
    server_2 = np.array(server_2)
    server_3 = np.array(server_3)
    server_4 = np.array(server_4)

    plot_vals(time, stable, 'Convergance', descriptor)
    plot_vals(time, buffer, 'Buffer size', descriptor)
    plot_vals(time, avg_serving, 'Average serving', descriptor)

    print(buffer.max())
    print(buffer.mean())

    #plot_vals(time, server_0, 'Serer0-incoming', descriptor)
    #plot_vals(time, server_1, 'Serer1-incoming', descriptor)
    #plot_vals(time, server_2, 'Serer2-incoming', descriptor)
    #plot_vals(time, server_3, 'Serer3-incoming', descriptor)
    #plot_vals(time, server_4, 'Serer4-incoming', descriptor)





