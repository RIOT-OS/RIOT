MODULE = tflite-micro-memory-planner

SRCXXEXT = cc
SRCXXEXCLUDE = $(wildcard *_test.$(SRCXXEXT))

include $(RIOTBASE)/Makefile.base
