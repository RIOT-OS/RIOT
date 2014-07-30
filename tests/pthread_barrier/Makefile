# name of your application
APPLICATION = pthread_barrier
include ../Makefile.tests_common

# exclude boards with insufficient RAM
BOARD_INSUFFICIENT_RAM :=  stm32f0discovery

## Modules to include.
USEMODULE += pthread
USEMODULE += random
USEMODULE += vtimer

CFLAGS += -isystem $(RIOTBASE)/sys/posix/pthread/include
CFLAGS += -isystem $(RIOTBASE)/sys/posix/include

INCLUDES += -I$(RIOTBASE)/sys/posix/pthread/include
INCLUDES += -I$(RIOTBASE)/sys/posix/include

CFLAGS += -ggdb3 -O0

include $(RIOTBASE)/Makefile.include
