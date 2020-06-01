#compiler
CC=gcc
#compilation flags
CFLAG= -g -Wall

# <tagert> <target dep>
# dep: recipe
server: server.o 
	$(CC) -o server server.c

.PHONY: run
run: server
	./server
clean: 
	rm -rf *.o server
