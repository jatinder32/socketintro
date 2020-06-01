# project setup
CC	= gcc
INCLDIR	= include/
BINDIR	= bin/
OBJDIR 	= obj/
SRCDIR	= src/

# final excutable name
_BIN 	= client
BIN	= $(addprefix $(BINDIR), $(_BIN))

#files
SRC	= $(wildcard src/*.c)
_OBJS	= $(patsubst src/%.c, %.o, $(SRC))
OBJS	= $(addprefix $(OBJDIR), $(_OBJS))

# compilation flags
CFLAGS = -Wall -std=c99 -g -I$(INCLDIR)

# compile binary and object files
.PHONY: all
all: $(BIN)

$(BIN): $(BINDIR) $(OBJS)
	$(CC) $(OBJS) -o $(BIN)

$(BINDIR):
	mkdir -p $(BINDIR)

$(OBJS): $(OBJDIR) $(SRC)
	$(CC) $(CFLAGS) -c $(SRC) -o $(OBJS)

$(OBJDIR):
	mkdir -p $(OBJDIR)
# clean entire project directory
.PHONY: clean
clean:
	- rm -rf $(BINDIR) $(OBJDIR)
