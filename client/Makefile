#compiler
CC=gcc
#compilation flags
CFLAG= -g -Wall

# <tagert> <target dep>
# dep: recipe
client: client.o 
	$(CC) -o client client.c

.PHONY: run
run: client
	./client
clean: 
	rm -rf *.o client
