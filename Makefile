# Creating the  executables
all: Simulator

Simulator:  ./*
	g++ -Wall -o2 -march=native ./main.cpp ./Dispacher.cpp ./MinHeap.cpp ./Server.cpp ./Job.cpp ./JBuffer.cpp -o LoadBalanceSimulator.exe -std=c++11


# Cleaning old files before new make
clean:
	rm -f LoadBalanceSimulator.exe 




