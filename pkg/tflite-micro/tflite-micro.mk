MODULE = tflite-micro

SRCXXEXT = cc
SRCXXEXCLUDE = $(wildcard *_test.$(SRCXXEXT))

include $(RIOTBASE)/Makefile.base
