CFLAGS += -Wall -Wextra
CFLAGS += -I../../../sys/include

OBJS := main.o serial.o

all: bin bin/riotboot_serial

%.o: %.c
	@cc $(CFLAGS) -c -o $@ $<

bin:
	mkdir bin

bin/riotboot_serial: $(OBJS)
	@cc -o $@ $(OBJS)

clean:
	@rm -fr $(OBJS)
	@rm -fr bin/riotboot_serial
