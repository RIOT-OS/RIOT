# name of your application
APPLICATION = pthread_barrier
include ../Makefile.tests_common

BOARD_BLACKLIST := arduino-mega2560
# arduino-mega2560: unknown type name: clockid_t

# exclude boards with insufficient memory
BOARD_INSUFFICIENT_MEMORY :=  stm32f0discovery

# Modules to include.
USEMODULE += pthread
USEMODULE += random
USEMODULE += vtimer

include $(RIOTBASE)/Makefile.include
