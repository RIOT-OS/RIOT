# name of your application
APPLICATION = msg_send_receive
include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := nucleo32-f031

include $(RIOTBASE)/Makefile.include

test:
	./tests/01-run.py
