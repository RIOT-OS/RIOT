CFLAGS?=-g -O3 -Wall -Wextra

BINARY := bin/benchmark_server
all: bin $(BINARY)

bin:
	mkdir bin

run:
	$(BINARY) :: 12345

RIOTBASE:=../../..
RIOT_INCLUDES=-I$(RIOTBASE)/core/include -I$(RIOTBASE)/sys/include
SRCS:=$(wildcard *.c)
$(BINARY): $(SRCS)
	$(CC) $(CFLAGS) $(CFLAGS_EXTRA) $(RIOT_INCLUDES) -I.. $(SRCS) -o $@

clean:
	rm -f $(BINARY)
