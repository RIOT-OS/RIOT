export PROJECT = test_irq
include ../Makefile.tests_common

ifeq (,$(filter native,$(BOARD)))
	include $(RIOTBASE)/Makefile.unsupported
else

## Modules to include.

USEMODULE += auto_init
USEMODULE += hwtimer
USEMODULE += posix

include $(RIOTBASE)/Makefile.include
endif
