export PROJECT = test_bloom
include ../Makefile.tests_common

ifneq (,$(filter msb-430,$(BOARD)))
include $(RIOTBASE)/Makefile.unsupported
else

USEMODULE += hashes
USEMODULE += bloom
USEMODULE += random

DISABLE_MODULE += auto_init

include $(RIOTBASE)/Makefile.include
endif
