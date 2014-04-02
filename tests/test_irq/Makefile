export PROJECT = test_irq
include ../Makefile.tests_common

ifeq (,$(filter native,$(BOARD)))
	include $(RIOTBASE)/Makefile.unsupported
else

USEMODULE += posix

include $(RIOTBASE)/Makefile.include
endif
