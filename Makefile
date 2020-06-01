CC=gcc
CFLAG= -g -Wall

server:= ./server
client:=./client

.PHONY: all $(server) $(client)
all: $(server) $(client)

$(server): 
	$(MAKE) --directory=$@

$(client): 
	$(MAKE) --directory=$@

.PHONY: clean
clean: $(server) $(client)
	rm $(server)/*.o $(client)/*.o
