BIN     =  sliptty
CFLAGS  += -O3 -Wall -pedantic

all: $(BIN)

debug: CFLAGS += -g3
debug: all

$(BIN): $(wildcard *.c)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	$(RM) $(BIN)

distclean: clean
