BOARD ?= nucleo-f767zi

# This needs to be here so that this app's `soft_uart_params.h` is found used
# of the one in $(RIOTBASE)/drivers/soft_uart/include.
INCLUDES += -I$(abspath $(CURDIR))/include

include ../Makefile.periph_common

USEMODULE += soft_uart
USEMODULE += fmt
# we do not need stdio
USEMODULE += stdio_null

# We do not need multi-threading for this app
DISBALE_MODULE += core_thread

include $(RIOTBASE)/Makefile.include
