include ../Makefile.tests_common

USEMODULE += xtimer

USEPKG += umorse

UMORSE_DELAY_DIT ?= 120
CFLAGS += -DUMORSE_DELAY_DIT=$(UMORSE_DELAY_DIT)

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
