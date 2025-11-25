CFLAGS = -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)"
CC = gcc
prefix=/usr/local

all: lpc2k_pgm pseudoterm

SRC  = lpc2k_pgm.c download.c uuencode.c ihex.c serial.c chipinfo.c boot_2xxx.c boot_23xx.c control_2xxx.c
OBJS = $(addprefix obj/,$(patsubst %.c,%.o,$(SRC)))

PSEUDOTERM_SRC = pseudoterm.c serial.c control_2xxx.c
PSEUDOTERM_OBJS = $(addprefix obj/,$(patsubst %.c,%.o,$(PSEUDOTERM_SRC)))

TARGETDIR = bin

lpc2k_pgm: $(OBJS)
	mkdir -p $(TARGETDIR)
	$(CC) -o $(TARGETDIR)/lpc2k_pgm $(OBJS)

pseudoterm: $(PSEUDOTERM_OBJS)
	mkdir -p $(TARGETDIR)
	$(CC) -o $(TARGETDIR)/pseudoterm $(PSEUDOTERM_OBJS) -lpthread

chipinfo.o: boot_2xxx.h boot_23xx.h

static: $(OBJS) mkstaticlist
	$(CC) -o $(TARGETDIR)/lpc2k_pgm $(OBJS) `./mkstaticlist`

obj/%.o : src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

cksum_test: cksum_test.c uuencode.o cksum_test.o
	$(CC) $(CFLAGS) -o $(TARGETDIR)/cksum_test obj/cksum_test.o obj/uuencode.o

boot_2xxx.c boot_2xxx.h: boot_2xxx.armasm mkbootc
	arm-none-eabi-as -o boot_2xxx.armobj boot_2xxx.armasm
	arm-none-eabi-objdump -d boot_2xxx.armobj | ./mkbootc boot_2xxx

boot_23xx.c boot_23xx.h: src/boot_23xx.armasm mkbootc
	arm-none-eabi-as -o obj/boot_23xx.armobj src/boot_23xx.armasm
	arm-none-eabi-objdump -d obj/boot_23xx.armobj | ./mkbootc boot_23xx

clean:
	rm -f bin/lpc2k_pgm cksum_test obj/*.o obj/*.d core core.* obj/*.armobj bin/pseudoterm

install:
	install -m 0755 bin/lpc2k_pgm $(prefix)/bin

.PHONY: install

obj/gui.o: src/gui.c
	$(CC) $(CFLAGS) `gtk-config --cflags` -c src/gui.c -o obj/gui.o
