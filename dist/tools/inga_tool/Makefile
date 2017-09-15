UNAME := $(shell uname)
FTDI_VERSION := -1

# this is ugly!
CC = gcc
ifeq ($(UNAME), Linux)
# Use pkg-config to determine the compiler and linker flag forlibudev
LIBUDEV_CFLAGS := `pkg-config --cflags libudev`
LIBUDEV_LDLIBS := `pkg-config --libs libudev`
endif

# Use pkg-config to determine which libftdi version we have
HASFTDI  := $(shell pkg-config --exists libftdi ; echo "$$?" )
HASFTDI0 := $(shell pkg-config --exists libftdi0; echo "$$?" )
HASFTDI1 := $(shell pkg-config --exists libftdi1; echo "$$?" )

# libftdi1 uses libusb-1.0, libftdi0 uses libusb (0.1)
ifeq (0, ${HASFTDI1})
	FTDI_VERSION := 1
	FTDI_CFLAGS  := `pkg-config --cflags libftdi1 libusb-1.0`
	FTDI_LDLIBS  := `pkg-config --libs libftdi1 libusb-1.0`
else ifeq (0, ${HASFTDI0})
	FTDI_VERSION := 0
	FTDI_CFLAGS  := `pkg-config --cflags libftdi0 libusb`
	FTDI_LDLIBS  := `pkg-config --libs libftdi0 libusb`
else ifeq (0, ${HASFTDI})
	FTDI_VERSION := 0
	FTDI_CFLAGS  := `pkg-config --cflags libftdi libusb`
	FTDI_LDLIBS  := `pkg-config --libs libftdi libusb`
endif


# Use pkg-config to determine the compiler and linker flag for popt
POPT_CFLAGS := `pkg-config --cflags popt`
POPT_LDLIBS := `pkg-config --libs popt`


CFLAGS=${FTDI_CFLAGS} ${POPT_CFLAGS} ${LIBUDEV_CFLAGS} -DFTDI_VERSION="${FTDI_VERSION}" -ggdb
LDLIBS=${FTDI_LDLIBS} ${POPT_LDLIBS} ${LIBUDEV_LDLIBS}

# Standard recipes to make inga_tool

NAME    := inga_tool
SOURCES := inga_usb.c inga_tool.c
HEADERS := inga_usb.h
OBJECTS := $(SOURCES:%.c=%.o)

all: ${NAME}

${NAME}: ${OBJECTS}
	$(info Making $@)
	${CC} ${OBJECTS} ${LDLIBS} -o $@

${OBJECTS}: %.o:%.c ${HEADERS}
	$(info Making $@)
	${CC} ${CFLAGS} $< -c -o $@

clean:
	rm -f ${OBJECTS} ${NAME}

.PHONY: clean
