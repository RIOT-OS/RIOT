include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := arduino-duemilanove arduino-uno

# chronos : USART_1 undeclared
BOARD_BLACKLIST += chronos
# mips-malta : undefined reference to uart_write
BOARD_BLACKLIST += mips-malta

USEMODULE += dynamixel
USEMODULE += shell

include $(RIOTBASE)/Makefile.include
