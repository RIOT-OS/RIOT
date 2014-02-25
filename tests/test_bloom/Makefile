export PROJECT = test_bloom
include ../Makefile.tests_common

ifneq (,$(filter msb-430,$(BOARD)))
	include $(RIOTBASE)/Makefile.unsupported
elifneq (,$(filter msb-430h,$(BOARD)))
	include $(RIOTBASE)/Makefile.unsupported
endif

USEMODULE += hashes
USEMODULE += bloom

export PROJDEPS = sets.h

include $(RIOTBASE)/Makefile.include

sets.h: generate_sets.py words.txt.gz
	./generate_sets.py 10000 20
