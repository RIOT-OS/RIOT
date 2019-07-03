include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := arduino-duemilanove arduino-leonardo arduino-nano \
                             arduino-uno

# chronos : USART_1 undeclared
BOARD_BLACKLIST += chronos

USEMODULE += dynamixel
USEMODULE += shell

include $(RIOTBASE)/Makefile.include
