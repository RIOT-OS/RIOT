APPLICATION = unittests
include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := airfy-beacon \
                             arduino-duemilanove \
                             arduino-mega2560 \
                             arduino-mkr1000 \
                             arduino-mkrzero \
                             arduino-uno \
                             arduino-zero \
                             avsextrem \
                             b-l072z-lrwan1 \
                             bluepill \
                             calliope-mini \
                             cc2538dk \
                             cc2650-launchpad \
                             cc2650stk \
                             chronos \
                             ek-lm4f120xl \
                             feather-m0 \
                             limifrog-v1 maple-mini \
                             mbed_lpc1768 \
                             microbit \
                             msb-430 \
                             msb-430h \
                             nrf51dongle \
                             nrf6310 \
                             nucleo32-f031 \
                             nucleo32-f042 \
                             nucleo32-f303 \
                             nucleo32-l031 \
                             nucleo32-l432 \
                             nucleo-f030 \
                             nucleo-f070 \
                             nucleo-f072 \
                             nucleo-f091 \
                             nucleo-f103 \
                             nucleo-f302 \
                             nucleo-f334 \
                             nucleo-f410 \
                             nucleo-l053 \
                             nucleo-l073 \
                             nz32-sc151 \
                             opencm904 \
                             openmote \
                             openmote-cc2538 \
                             pba-d-01-kw2x \
                             remote-pa \
                             remote-reva \
                             remote-revb \
                             saml21-xpro \
                             samd21-xpro \
                             samr21-xpro \
                             seeeduino_arch-pro \
                             slwstk6220a \
                             sodaq-autonomo \
                             sodaq-explorer \
                             spark-core \
                             stm32f0discovery \
                             stm32f3discovery \
                             telosb \
                             waspmote-pro \
                             wsn430-v1_3b \
                             wsn430-v1_4 \
                             yunjia-nrf51822 z1

USEMODULE += embunit

ifeq (, $(filter tests-%, $(MAKECMDGOALS)))
  # the $(dir) Makefile function leaves a trailing slash after the directory
  # name, therefore we use patsubst instead.
  UNIT_TESTS := $(patsubst %/Makefile,%,$(wildcard tests-*/Makefile))
else
  UNIT_TESTS := $(filter tests-%, $(MAKECMDGOALS))
endif

ARM7_BOARDS := msba2 avrextrem
DISABLE_TEST_FOR_ARM7 := tests-relic tests-cpp_%

ARM_CORTEX_M_BOARDS := airfy-beacon \
                       arduino-due \
                       arduino-mkr1000 \
                       arduino-mkrzero \
                       arduino-zero \
                       b-l072z-lrwan1 \
                       cc2538dk \
                       ek-lm4f120xl \
                       f4vi1 \
                       feather-m0 \
                       fox \
                       frdm-k64f \
                       iotlab-m3 \
                       limifrog-v1 \
                       mbed_lpc1768 \
                       msbiot \
                       mulle \
                       nrf51dongle \
                       nrf52840dk \
                       nrf6310 \
                       nucleo144-f303 \
                       nucleo144-f412 \
                       nucleo144-f429 \
                       nucleo144-f446 \
                       nucleo32-f031 \
                       nucleo32-f303 \
                       nucleo32-l031 \
                       nucleo32-l432 \
                       nucleo-f030 \
                       nucleo-f070 \
                       nucleo-f072 \
                       nucleo-f091 \
                       nucleo-f302 \
                       nucleo-f303 \
                       nucleo-f334 \
                       nucleo-f401 \
                       nucleo-f410 \
                       nucleo-f411 \
                       nucleo-l053 \
                       nucleo-l073 \
                       nucleo-l152 \
                       nucleo-l476 \
                       nz32-sc151 \
                       opencm904 \
                       openmote-cc2538 \
                       pba-d-01-kw2x \
                       remote \
                       samd21-xpro \
                       saml21-xpro \
                       samr21-xpro \
                       slwstk6220a \
                       sodaq-autonomo \
                       sodaq-explorer \
                       spark-core \
                       stm32f0discovery \
                       stm32f3discovery \
                       stm32f4discovery \
                       udoo \
                       yunjia-nrf51822

DISABLE_TEST_FOR_ARM_CORTEX_M := tests-relic

AVR_BOARDS := arduino-mega2560 waspmote-pro arduino-uno arduino-duemilanove
DISABLE_TEST_FOR_AVR := tests-relic tests-cpp_%

MSP430_BOARDS :=  chronos msb-430 msb-430h telosb wsn430-v1_3b wsn430-v1_4 z1
DISABLE_TEST_FOR_MSP430 := tests-relic tests-spiffs tests-cpp_%

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
  UNIT_TESTS := $(filter-out $(DISABLE_TEST_FOR_MSP430), $(UNIT_TESTS))
endif

ifneq (,$(filter tests-cpp_%, $(UNIT_TESTS)))
  # We need to tell the build system to use the C++ compiler for linking
  export FEATURES_REQUIRED += cpp
  export CPPMIX := 1
endif

DISABLE_MODULE += auto_init

# Pull in `Makefile.include`s from the test suites:
-include $(UNIT_TESTS:%=$(RIOTBASE)/tests/unittests/%/Makefile.include)

DIRS += $(UNIT_TESTS)
BASELIBS += $(UNIT_TESTS:%=$(BINDIR)/%.a)

INCLUDES += -I$(RIOTBASE)/tests/unittests/common

include $(RIOTBASE)/Makefile.include

.PHONY: $(UNIT_TESTS)

all:

$(UNIT_TESTS): all

charCOMMA := ,

ifeq (, $(UNIT_TESTS))
  CFLAGS += -DNO_TEST_SUITES
  $(warning There was no test suite specified!)
else
  CFLAGS += -DTEST_SUITES='$(subst $() $(),$(charCOMMA),$(UNIT_TESTS:tests-%=%))'
endif

test:
	./tests/01-run.py
