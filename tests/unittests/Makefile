APPLICATION = unittests
include ../Makefile.tests_common

BOARD_INSUFFICIENT_RAM := chronos msb-430 msb-430h redbee-econotag stm32f0discovery \
                          telosb wsn430-v1_3b wsn430-v1_4 z1 nucleo-f334

USEMODULE += embunit

# Some randomly generated but still deterministic values for testing
CFLAGS += -DTEST_STRING4="\"J&(d\""
CFLAGS += -DTEST_STRING8="\"o<\\\\rrB/q\""
CFLAGS += -DTEST_STRING12="\"50U'HLKC3_ft\""
CFLAGS += -DTEST_STRING16="\"nvxuO*6o3C=a6g7]\""
CFLAGS += -DTEST_STRING64="\"3key9'=4\\\\wvAM&;.E&U/rOG:>\\\\C!r{K k.E[%;(}_I(D%XQk8+T>_M^G'Mm49,n:))]}\""
CFLAGS += -DTEST_INT8=-69
CFLAGS += -DTEST_INT16=-12229
CFLAGS += -DTEST_INT32=-1894529023L
CFLAGS += -DTEST_INT64=-4562349816927799244LL
CFLAGS += -DTEST_UINT8=95U
CFLAGS += -DTEST_UINT16=10098U
CFLAGS += -DTEST_UINT32=2831907245LU
CFLAGS += -DTEST_UINT64=13500266397057512199LLU

ifeq (, $(filter tests-%, $(MAKECMDGOALS)))
    UNIT_TESTS := $(foreach d,$(wildcard tests-*/Makefile),$(shell dirname $(d)))
else
    UNIT_TESTS := $(filter tests-%, $(MAKECMDGOALS))
endif

DISABLE_MODULE += auto_init

# Pull in `Makefile.include`s from the test suites:
-include $(UNIT_TESTS:%=$(RIOTBASE)/tests/unittests/%/Makefile.include)

ifneq (,$(filter netdev_dummy,$(USEMODULE)))
    USEMODULE += netdev_base
    DIRS += netdev_dummy
    INCLUDES += -I$(RIOTBASE)/tests/unittests/netdev_dummy/include
endif

DIRS += $(UNIT_TESTS)
BASELIBS += $(UNIT_TESTS:%=$(BINDIR)%.a)

include $(RIOTBASE)/Makefile.include

ifeq (,$(filter msp430fxyz,$(CPU)))
    CFLAGS += -DTEST_INT=TEST_INT32
else
    CFLAGS += -DTEST_INT=TEST_INT16
endif

.PHONY: FORCE $(UNIT_TESTS)

FORCE:
	touch $(CURDIR)/main.c

all: FORCE

$(UNIT_TESTS): FORCE all

charCOMMA := ,

ifeq (, $(UNIT_TESTS))
    CFLAGS += -DNO_TEST_SUITES
    $(warning There was no test suite specified!)
else
    CFLAGS += -DTEST_SUITES='$(subst $() $(),$(charCOMMA),$(UNIT_TESTS:tests-%=%))'
endif

test: SHELL=bash
test:
	@exec 5>&1 && \
	LOG=$$("$(MAKE)" -s term | tee >(cat - >&5)) && \
	grep 'OK ([1-9][0-9]* tests)' <<< $${LOG} > /dev/null
