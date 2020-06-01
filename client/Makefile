#compiler
CC=gcc
#compilation flags
CFLAG= -g -Wall

# <tagert> <target dep>
# dep: recipe
server: client.o 
	$(CC) -o client client.c

clean: 
	rm -rf *.o client
