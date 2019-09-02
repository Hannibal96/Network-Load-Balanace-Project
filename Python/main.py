import matplotlib.pyplot as plt
import sys

x = open("./../results/Servers-10,0.01;10,0.1;5,1;-JIQ-Load-0.950000-Buffer-Low=5-High=20_values.log")
buffer = list()
stable = list()

for idx, line in enumerate(x):
    splited = line.split()
    if idx % 10000 == 0 :
        print(idx)
    buffer.append(splited[2])
    stable.append(splited[1])

print(len(buffer))
plt.plot(stable)
plt.show()

plt.plot(buffer)
plt.show()