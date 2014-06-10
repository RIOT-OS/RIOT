# name of your application
APPLICATION = pthread_barrier
include ../Makefile.tests_common

# exclude boards with insufficient RAM
BOARD_INSUFFICIENT_RAM :=  stm32f0discovery

# Modules to include.
USEMODULE += pthread
USEMODULE += random
USEMODULE += vtimer

include $(RIOTBASE)/Makefile.include
