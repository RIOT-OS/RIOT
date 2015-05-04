APPLICATION = unittests
include ../Makefile.tests_common

BOARD_INSUFFICIENT_RAM := airfy-beacon chronos msb-430 msb-430h pca10000 \
                          pca10005 redbee-econotag spark-core stm32f0discovery \
                          telosb wsn430-v1_3b wsn430-v1_4 z1 nucleo-f334 \
                          yunjia-nrf51822 samr21-xpro arduino-mega2560

USEMODULE += embunit

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

INCLUDES += -I$(RIOTBASE)/tests/unittests/common

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
