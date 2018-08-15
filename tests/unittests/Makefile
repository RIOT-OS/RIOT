DEVELHELP ?= 0
include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := airfy-beacon \
                             arduino-duemilanove \
                             arduino-mega2560 \
                             arduino-mkr1000 \
                             arduino-mkrfox1200 \
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
                             ikea-tradfri \
                             limifrog-v1 maple-mini \
                             mbed_lpc1768 \
                             mega-xplained \
                             microbit \
                             msb-430 \
                             msb-430h \
                             msba2 \
                             nrf51dongle \
                             nrf6310 \
                             nucleo-f031k6 \
                             nucleo-f042k6 \
                             nucleo-f303k8 \
                             nucleo-l031k6 \
                             nucleo-l432kc \
                             nucleo-f030r8 \
                             nucleo-f070rb \
                             nucleo-f072rb \
                             nucleo-f091rc \
                             nucleo-f103rb \
                             nucleo-f302r8 \
                             nucleo-f334r8 \
                             nucleo-f410rb \
                             nucleo-l053r8 \
                             nucleo-l073rz \
                             nucleo-l433rc \
                             nz32-sc151 \
                             opencm904 \
                             openmote \
                             openmote-cc2538 \
                             openmote-b \
                             pba-d-01-kw2x \
                             remote-pa \
                             remote-reva \
                             remote-revb \
                             saml21-xpro \
                             samd21-xpro \
                             samr21-xpro \
                             seeeduino_arch-pro \
                             sensebox_samd21 \
                             slstk3401a \
                             sltb001a \
                             slwstk6220a \
                             sodaq-autonomo \
                             sodaq-explorer \
                             sodaq-one \
                             spark-core \
                             stk3600 \
                             stm32f0discovery \
                             stm32f3discovery \
                             stm32mindev \
                             teensy31 \
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

ARM7_BOARDS := avsextrem \
               msba2 \
               #
DISABLE_TEST_FOR_ARM7 := tests-relic tests-cpp_%

ARM_CORTEX_M_BOARDS := airfy-beacon \
                       arduino-due \
                       arduino-mkr1000 \
                       arduino-mkrfox1200 \
                       arduino-mkrzero \
                       arduino-zero \
                       b-l072z-lrwan1 \
                       bluepill \
                       calliope-mini \
                       cc2538dk \
                       cc2650-launchpad \
                       cc2650stk \
                       ek-lm4f120xl \
                       f4vi1 \
                       feather-m0 \
                       fox \
                       frdm-k22f \
                       frdm-k64f \
                       frdm-kw41z \
                       ikea-tradfri \
                       iotlab-a8-m3 \
                       iotlab-m3 \
                       limifrog-v1 \
                       maple-mini \
                       mbed_lpc1768 \
                       microbit \
                       msbiot \
                       mulle \
                       nrf51dongle \
                       nrf52840dk \
                       nrf52dk \
                       nrf6310 \
                       nucleo-f207zg \
                       nucleo-f303ze \
                       nucleo-f412zg \
                       nucleo-f413zh \
                       nucleo-f429zi \
                       nucleo-f446ze \
                       nucleo-f722ze \
                       nucleo-f746zg \
                       nucleo-f767zi \
                       nucleo-f031k6 \
                       nucleo-f042k6 \
                       nucleo-f303k8 \
                       nucleo-l031k6 \
                       nucleo-l432kc \
                       nucleo-f030r8 \
                       nucleo-f070rb \
                       nucleo-f072rb \
                       nucleo-f091rc \
                       nucleo-f103rb \
                       nucleo-f302r8 \
                       nucleo-f303re \
                       nucleo-f334r8 \
                       nucleo-f401re \
                       nucleo-f410rb \
                       nucleo-f411re \
                       nucleo-f446re \
                       nucleo-l053r8 \
                       nucleo-l073rz \
                       nucleo-l152re \
                       nucleo-l433rc \
                       nucleo-l476rg \
                       nucleo-l496zg \
                       nz32-sc151 \
                       opencm904 \
                       openmote-cc2538 \
                       pba-d-01-kw2x \
                       remote-pa \
                       remote-reva \
                       remote-revb \
                       samd21-xpro \
                       saml21-xpro \
                       samr21-xpro \
                       seeeduino_arch-pro \
                       slstk3401a \
                       slstk3402a \
                       sltb001a \
                       slwstk6000b \
                       slwstk6220a \
                       sodaq-autonomo \
                       sodaq-explorer \
                       sodaq-one \
                       spark-core \
                       stk3600 \
                       stk3700 \
                       stm32f0discovery \
                       stm32f3discovery \
                       stm32f4discovery \
                       stm32f769i-disco \
                       udoo \
                       yunjia-nrf51822 \
                       #
DISABLE_TEST_FOR_ARM_CORTEX_M := tests-relic

AVR_BOARDS := arduino-duemilanove \
              arduino-mega2560 \
              arduino-uno \
              waspmote-pro \
              #
DISABLE_TEST_FOR_AVR := tests-relic tests-cpp_%

MSP430_BOARDS := chronos \
                 msb-430 \
                 msb-430h \
                 telosb \
                 wsn430-v1_3b \
                 wsn430-v1_4 \
                 z1 \
                 #
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

ifneq (, $(filter $(AVR_BOARDS), $(BOARD)))
  LARGE_STACK_TESTS += tests-qDSA
endif

LARGE_STACK_TESTS += tests-hacl
LARGE_STACK_TESTS += tests-libcose
LARGE_STACK_TESTS += tests-tweetnacl
ifneq (,$(filter $(LARGE_STACK_TESTS), $(UNIT_TESTS)))
  CFLAGS += -DTHREAD_STACKSIZE_MAIN=\(4*THREAD_STACKSIZE_DEFAULT+THREAD_EXTRA_STACKSIZE_PRINTF\)
endif

DISABLE_MODULE += auto_init

# Pull in `Makefile.include`s from the test suites:
-include $(UNIT_TESTS:%=$(RIOTBASE)/tests/unittests/%/Makefile.include)

DIRS += $(UNIT_TESTS)
BASELIBS += $(UNIT_TESTS:%=$(BINDIR)/%.a)

INCLUDES += -I$(RIOTBASE)/tests/unittests/common

# list of boards to run CI tests on
TEST_ON_CI_WHITELIST += all

include $(RIOTBASE)/Makefile.include

.PHONY: $(UNIT_TESTS)

all:

info-unittests:
	@echo $(UNIT_TESTS)

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
