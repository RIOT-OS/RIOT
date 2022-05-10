MODULE = ruy

SRCXXEXT = cc
SRCXXEXCLUDE = $(wildcard *_test.$(SRCXXEXT))
SRCXXEXCLUDE += benchmark.cc pack_%.cc test_%.cc

CFLAGS += -Wno-unused-variable

include $(RIOTBASE)/Makefile.base
