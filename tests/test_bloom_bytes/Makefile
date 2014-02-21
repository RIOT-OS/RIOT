export PROJECT = test_bloom
include ../Makefile.tests_common

ifneq (,$(filter msb-430,$(BOARD)))
include $(RIOTBASE)/Makefile.unsupported
else

## Modules to include.
USEMODULE += hashes
USEMODULE += bloom
USEMODULE += random

include $(RIOTBASE)/Makefile.include
endif
