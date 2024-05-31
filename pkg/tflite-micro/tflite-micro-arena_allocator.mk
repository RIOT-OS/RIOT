MODULE = tflite-micro-arena_allocator

SRCXXEXT = cc
SRCXXEXCLUDE = $(wildcard *_test.$(SRCXXEXT))

include $(RIOTBASE)/Makefile.base
