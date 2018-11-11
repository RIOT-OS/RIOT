include ../Makefile.tests_common

USEMODULE += pir

all-polling: CFLAGS += -DTEST_PIR_POLLING

all-polling: all

all-interrupt: all

BOARD_INSUFFICIENT_MEMORY := \
  arduino-duemilanove \
  arduino-uno \
  nucleo-f031k6 \
  #

include $(RIOTBASE)/Makefile.include
