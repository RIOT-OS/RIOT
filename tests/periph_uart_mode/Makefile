include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := arduino-duemilanove arduino-leonardo arduino-nano\
                             arduino-uno nucleo-f031k6

FEATURES_REQUIRED += periph_uart
FEATURES_REQUIRED += periph_uart_modecfg

USEMODULE += xtimer

# Set this to prevent welcome message from printing and confusing output
CFLAGS+=-DLOG_LEVEL=LOG_NONE

include $(RIOTBASE)/Makefile.include
