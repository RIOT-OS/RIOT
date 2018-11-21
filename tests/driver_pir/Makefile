include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := arduino-duemilanove arduino-uno nucleo-f031k6

USEMODULE += pir

include $(RIOTBASE)/Makefile.include

all-polling: CFLAGS += -DTEST_PIR_POLLING

all-polling: all

all-interrupt: all
