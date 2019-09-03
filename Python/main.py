import matplotlib.pyplot as plt
import sys

x = open("./../results/Servers-10,0.01;10,0.1;5,1;-Random-Load-0.990000-Buffer-Low=4-High=8_values.log")
time = list()
avg_serving = list()
buffer = list()
stable = list()

for idx, line in enumerate(x):
    splited = line.split()
    if idx % 10000 == 0 :
        print(idx)
    time.append(splited[1])
    avg_serving.append(splited[3])
    stable.append(splited[5])
    buffer.append(splited[7])

print(len(buffer))
plt.plot(stable)
plt.title("Convergance")
plt.show()
plt.plot(buffer)
plt.title("Buffer")
plt.show()
plt.plot(avg_serving)
plt.title("Serving")
plt.show()
