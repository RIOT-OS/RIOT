include ../Makefile.tests_common

# chronos : USART_1 undeclared
BOARD_BLACKLIST += chronos

# mips-malta : undefined reference to uart_write
BOARD_BLACKLIST += mips-malta

USEMODULE += feetech
USEMODULE += shell

include $(RIOTBASE)/Makefile.include
