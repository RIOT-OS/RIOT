MODULE = tflite-micro-kernels

SRCXXEXT = cc
SRCXXEXCLUDE = $(wildcard *_test.$(SRCXXEXT))

include $(RIOTBASE)/Makefile.base
