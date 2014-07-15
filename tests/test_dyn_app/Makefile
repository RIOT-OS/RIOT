
BOARD ?= msb-430
RIOTBASE = $(CURDIR)/../..
RIOTBOARD = $(RIOTBASE)/boards
RIOTCPU = $(RIOTBASE)/cpu

include $(RIOTBOARD)/$(BOARD)/Makefile.include
include $(RIOTCPU)/$(CPU)/Makefile.include

all:
	$(AD)$(CC) -c main.c -o main.o
	hexdump -v -e '1/1 "0x%x" ", "' main.o | sed 's/\(.*\)..$$/const uint16_t _force_align = 10;const unsigned char dyn_app\[\] = \{\1\}\;\n/' > dyn_main.h


clean:
	rm -f main.o dyn_main.h
