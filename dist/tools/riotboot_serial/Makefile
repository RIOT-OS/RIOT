CFLAGS += -Wall -Wextra
CFLAGS += -I../../../sys/include

OBJS := main.o serial.o

%.o: %.c
	@cc $(CFLAGS) -c -o $@ $<

riotboot_serial: $(OBJS)
	@cc -o $@ $(OBJS)

clean:
	@rm -fr $(OBJS)
	@rm -fr riotboot_serial
