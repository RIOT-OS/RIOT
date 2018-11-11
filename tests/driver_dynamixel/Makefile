include ../Makefile.tests_common

USEMODULE += dynamixel
USEMODULE += shell

BOARD_INSUFFICIENT_MEMORY := \
  arduino-duemilanove \
  arduino-uno \
  #

# chronos : USART_1 undeclared
BOARD_BLACKLIST := \
  chronos \
  #

# mips-malta : undefined reference to uart_write
BOARD_BLACKLIST := \
  mips-malta \
  #

include $(RIOTBASE)/Makefile.include
