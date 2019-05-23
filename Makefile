CC = gcc
CFLAGS = -Wall -ansi -pedantic -g
MAIN = myfw
OBJS = myfw.o
all : $(MAIN)

$(MAIN) : $(OBJS) myfw.h
	$(CC) $(CFLAGS) -o $(MAIN) $(OBJS)

myfw.o : myfw.c myfw.h
	$(CC) $(CFLAGS) -c myfw.c

clean: 
	rm *.o $(MAIN) core
