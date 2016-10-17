APPLICATION = test-tomsfastmath
include ../Makefile.tests_common
USEPKG += tomsfastmath

CFLAGS += -DTHREAD_STACKSIZE_MAIN=8192 -DFP_MAX_SIZE=2080

include $(RIOTBASE)/Makefile.include

test:
	./tests/01-run.py
