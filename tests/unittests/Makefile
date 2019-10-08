DEVELHELP ?= 0
include ../Makefile.tests_common

# Issue with integer width
# There are present for a long time but hidden by being not compiled
BOARD_BLACKLIST += arduino-duemilanove
BOARD_BLACKLIST += arduino-leonardo
BOARD_BLACKLIST += arduino-mega2560
BOARD_BLACKLIST += arduino-nano
BOARD_BLACKLIST += arduino-uno
BOARD_BLACKLIST += chronos
BOARD_BLACKLIST += mega-xplained
BOARD_BLACKLIST += msb-430
BOARD_BLACKLIST += msb-430h
BOARD_BLACKLIST += telosb
BOARD_BLACKLIST += waspmote-pro
BOARD_BLACKLIST += wsn430-v1_3b
BOARD_BLACKLIST += wsn430-v1_4
BOARD_BLACKLIST += z1

BOARD_INSUFFICIENT_MEMORY := airfy-beacon \
                             arduino-duemilanove \
                             arduino-leonardo \
                             arduino-mega2560 \
                             arduino-mkr1000 \
                             arduino-mkrfox1200 \
                             arduino-mkrwan1300 \
                             arduino-mkrzero \
                             arduino-nano \
                             arduino-uno \
                             arduino-zero \
                             b-l072z-lrwan1 \
                             blackpill \
                             blackpill-128kib \
                             bluepill \
                             bluepill-128kib \
                             calliope-mini \
                             cc2650-launchpad \
                             cc2650stk \
                             chronos \
                             esp8266-esp-12x \
                             esp8266-olimex-mod \
                             esp8266-sparkfun-thing \
                             ek-lm4f120xl \
                             feather-m0 \
                             hamilton \
                             hifive1 \
                             hifive1b \
                             i-nucleo-lrwan1 \
                             ikea-tradfri \
                             limifrog-v1 maple-mini \
                             lobaro-lorabox \
                             lsn50 \
                             mega-xplained \
                             microbit \
                             msb-430 \
                             msb-430h \
                             nrf51dk \
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
                             pba-d-01-kw2x \
                             saml10-xpro \
                             saml11-xpro \
                             saml21-xpro \
                             samd21-xpro \
                             samr21-xpro \
                             samr30-xpro \
                             samr34-xpro \
                             sensebox_samd21 \
                             slstk3401a \
                             sltb001a \
                             slwstk6000b-slwrb4150a \
                             slwstk6220a \
                             sodaq-autonomo \
                             sodaq-explorer \
                             sodaq-one \
                             sodaq-sara-aff \
                             spark-core \
                             stk3600 \
                             stm32f0discovery \
                             stm32f3discovery \
                             stm32l0538-disco \
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

DISABLE_MODULE += auto_init

# Pull in `Makefile.include`s from the test suites:
-include $(UNIT_TESTS:%=$(RIOTBASE)/tests/unittests/%/Makefile.include)

DIRS += $(UNIT_TESTS)
BASELIBS += $(UNIT_TESTS:%=$(BINDIR)/%.a)

INCLUDES += -I$(RIOTBASE)/tests/unittests/common

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
