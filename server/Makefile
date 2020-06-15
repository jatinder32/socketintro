# project setup
CC	= gcc
INCLDIR	= ./include/
BINDIR	= bin/
OBJDIR 	= obj/
SRCDIR	= src/

# final excutable name
_BIN 	= a.out
BIN	= $(addprefix $(BINDIR), $(_BIN))

#files
SRC	= $(wildcard src/*.c)
_OBJS	= $(patsubst src/%.c, %.o, $(SRC))
OBJS	= $(addprefix $(OBJDIR), $(_OBJS))

LIBS= -lpthread
# compilation flags
CFLAGS = -Wall  -Wno-unused-variable  -DLOG_USE_COLOR -g -I$(INCLDIR)
OFLAGS =

# compile binary and object files
.PHONY: all
all: $(BIN)
$(BIN): $(BINDIR) $(OBJS)
	$(CC) $(OFLAGS) $(OBJS) -o $(BIN) $(LIBS)

$(BINDIR):
	mkdir -p $(BINDIR)
$(OBJS): $(OBJDIR) $(SRC)
	$(CC) $(CFLAGS) -c $(SRC)
	mv *.o $(OBJDIR) 

$(OBJDIR):
	mkdir -p $(OBJDIR)
# clean entire project directory
.PHONY: clean
clean:
	- rm -rf $(BINDIR) $(OBJDIR)
