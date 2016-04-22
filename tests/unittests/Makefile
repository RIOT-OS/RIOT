APPLICATION = unittests
include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := airfy-beacon cc2650stk chronos msb-430 msb-430h pca10000 \
                          pca10005 spark-core stm32f0discovery \
                          telosb wsn430-v1_3b wsn430-v1_4 z1 nucleo-f103 \
                          nucleo-f334 yunjia-nrf51822 samr21-xpro \
                          arduino-mega2560 airfy-beacon nrf51dongle nrf6310 weio nucleo-f072

USEMODULE += embunit

ifeq (, $(filter tests-%, $(MAKECMDGOALS)))
    UNIT_TESTS := $(foreach d,$(wildcard tests-*/Makefile),$(shell dirname $(d)))
else
    UNIT_TESTS := $(filter tests-%, $(MAKECMDGOALS))
endif

ARM7_BOARDS := msba2 avrextrem
DISABLE_TEST_FOR_ARM7 := tests-relic

ARM_CORTEX_M_BOARDS := airfy-beacon arduino-due cc2538dk ek-lm4f120xl f4vi1 fox frdm-k64f \
                       iotlab-m3 limifrog-v1 mbed_lpc1768 msbiot mulle nrf51dongle nrf6310 \
                       nucleo-f091 nucleo-f303 nucleo-f334 nucleo-f401 nucleo-l1 openmote-cc2538 \
                       pba-d-01-kw2x pca10000 pca10005 remote saml21-xpro samr21-xpro slwstk6220a \
                       spark-core stm32f0discovery stm32f3discovery stm32f4discovery udoo weio \
                       yunjia-nrf51822
DISABLE_TEST_FOR_ARM_CORTEX_M := tests-relic

AVR_BOARDS := arduino-mega2560
DISABLE_TEST_FOR_AVR := tests-relic

MSP430_BOARDS :=  chronos msb-430 msb-430h telosb wsn430-v1_3b wsn430-v1_4 z1
DISABLE_TEST_FOR_MSP430 := tests-relic

ifneq (, $(filter $(ARM7_BOARDS), $(BOARD)))
UNIT_TESTS := $(filter-out $(DISABLE_TEST_FOR_ARM7), $(UNIT_TESTS))
endif

ifneq (, $(filter $(ARM_CORTEX_M_BOARDS), $(BOARD)))
UNIT_TESTS := $(filter-out $(DISABLE_TEST_FOR_ARM_CORTEX_M), $(UNIT_TESTS))
endif

ifneq (, $(filter $(AVR_BOARDS), $(BOARD)))
UNIT_TESTS := $(filter-out $(DISABLE_TEST_FOR_AVR), $(UNIT_TESTS))
endif

ifneq (, $(filter $(MSP430_BOARDS), $(BOARD)))
UNIT_TESTS := $(filter-out $(DISABLE_TEST_FOR_AVR), $(UNIT_TESTS))
endif

DISABLE_MODULE += auto_init

# Pull in `Makefile.include`s from the test suites:
-include $(UNIT_TESTS:%=$(RIOTBASE)/tests/unittests/%/Makefile.include)

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

test:
	./tests/01-run.py
