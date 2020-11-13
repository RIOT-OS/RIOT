CFLAGS?=-g -O3 -Wall -Wextra

BINARY := bin/zep_dispatch
all: bin $(BINARY)

bin:
	mkdir bin

RIOTBASE:=../../..
RIOT_INCLUDE=$(RIOTBASE)/core/include
SRCS:=$(wildcard *.c)
$(BINARY): $(SRCS)
	$(CC) $(CFLAGS) $(CFLAGS_EXTRA) -I$(RIOT_INCLUDE) $(SRCS) -o $@

clean:
	rm -f $(BINARY)
