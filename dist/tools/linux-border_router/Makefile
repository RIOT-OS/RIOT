CFLAGS = -lrt -pthread -Wall
CC = gcc
DOCTOOL = doxygen

TESTING = -D BORDER_TESTING

all: sixlowdriver doc

SRC = main.c sixlowdriver.c serial.c control_2xxx.c multiplex.c flowcontrol.c serialnumber.c

TARGETDIR = ../../bin/linux
DOCDIR = ../../Documentation/linux

sixlowdriver: $(SRC)
	mkdir -p $(TARGETDIR) &> /dev/null
	$(CC) $(CFLAGS) -o $(TARGETDIR)/sixlowpan $(SRC)

sixlowtest: $(SRC) testing.c
	mkdir -p $(TARGETDIR) &> /dev/null
	$(CC) $(CFLAGS) $(TESTING) -o $(TARGETDIR)/sixlowpan $(SRC) testing.c

doc: $(SRC)
	mkdir -p $(DOCDIR) &> /dev/null
	$(DOCTOOL) > /dev/null
