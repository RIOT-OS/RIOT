include ../Makefile.tests_common

# This application uses getchar and thus expects input from stdio
USEMODULE += stdin
USEMODULE += hashes
USEMODULE += base64

ifeq (,$(filter native,$(BOARD)))
  RIOT_TERMINAL = other
  TERMPROG = miniterm.py
  TERMFLAGS = --eol LF /dev/ttyACM0 115200
endif

# boards fail due to UART weirdness and line buffering done by the terminal
BOARD_WHITELIST += native

include $(RIOTBASE)/Makefile.include
