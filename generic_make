CC = g++
OBJS = Server.o MinHeap.o Job.o JBuffer.o Dispacher.o main.o 
EXEC = LoadBalanceSimulator
DEBUG_FLAG = -g
COMP_FLAG = -Wall -std=c++0x

$(EXEC) : $(OBJS)
	$(CC) $(DEBUG_FLAG) $(OBJS) -o $@

Server.o: Server.cpp Server.h
	$(CC) -c $(DEBUG_FLAG) $(COMP_FLAG) $*.cpp

MinHeap.o: MinHeap.cpp MinHeap.h defs.h
	$(CC) -c $(DEBUG_FLAG) $(COMP_FLAG) $*.cpp

Job.o: Job.cpp Job.h defs.h
	$(CC) -c $(DEBUG_FLAG) $(COMP_FLAG) $*.cpp

JBuffer.o: JBuffer.cpp JBuffer.h Job.h Server.h defs.h
	$(CC) -c $(DEBUG_FLAG) $(COMP_FLAG) $*.cpp

Dispatcher.o: Dispacher.cpp Dispacher.h MinHeap.h Server.h Server.h JBuffer.h defs.h
	$(CC) -c $(DEBUG_FLAG) $(COMP_FLAG) $*.cpp

main.o: main.cpp Server.h Dispacher.h Job.h defs.h 
	$(CC) -c $(DEBUG_FLAG) $(COMP_FLAG) $*.cpp

clean:
	rm -f $(OBJS) $(EXEC)
	
