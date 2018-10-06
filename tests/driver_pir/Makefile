BOARD_INSUFFICIENT_MEMORY := nucleo-f031k6

USEMODULE += pir

include ../Makefile.tests_common
include $(RIOTBASE)/Makefile.include

all-polling: CFLAGS += -DTEST_PIR_POLLING

all-polling: all

all-interrupt: all
