APPLICATION = unittests
include ../Makefile.tests_common

BOARD_INSUFFICIENT_RAM := chronos msb-430 msb-430h redbee-econotag stm32f0discovery

USEMODULE += embunit

INCLUDES += -I$(RIOTBASE)/tests/unittests/embunit

ifeq ($(OUTPUT),XML)
	CFLAGS += -DOUTPUT=OUTPUT_XML
	USEMODULE += embunit_textui
else ifeq ($(OUTPUT),TEXT)
	CFLAGS += -DOUTPUT=OUTPUT_TEXT
	USEMODULE += embunit_textui
else ifeq ($(OUTPUT),COMPILER)
	CFLAGS += -DOUTPUT=OUTPUT_COMPILER
	USEMODULE += embunit_textui
endif

ifeq (, $(filter tests-%, $(MAKECMDGOALS)))
    UNIT_TESTS := $(foreach d,$(wildcard tests-*/Makefile),$(shell dirname $(d)))
else
    UNIT_TESTS := $(filter tests-%, $(MAKECMDGOALS))
endif

DISABLE_MODULE += auto_init

# Pull in `Makefile.include`s from the test suites:
-include $(UNIT_TESTS:%=$(RIOTBASE)/tests/unittests/%/Makefile.include)

DIRS += $(UNIT_TESTS)
BASELIBS += $(UNIT_TESTS:%=$(BINDIR)%.a)

include $(RIOTBASE)/Makefile.include

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
