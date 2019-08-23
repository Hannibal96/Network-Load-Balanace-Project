CC = g++
OBJS = main.o
EXEC = Parser 
DEBUG_FLAG = -g
COMP_FLAG = -Wall -std=c++0x

$(EXEC) : $(OBJS)
	$(CC) $(DEBUG_FLAG) $(OBJS) -o $@

main.o: main.cpp 
	$(CC) -c $(DEBUG_FLAG) $(COMP_FLAG) $*.cpp

clean:
	rm -f $(OBJS) $(EXEC)
	
