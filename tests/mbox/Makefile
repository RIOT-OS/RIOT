APPLICATION = mbox
include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := 

USEMODULE += mbox
USEMODULE += sema

DISABLE_MODULE += auto_init

CFLAGS = -DDEVELHELP

include $(RIOTBASE)/Makefile.include

test:
	python tests/01-run.py
