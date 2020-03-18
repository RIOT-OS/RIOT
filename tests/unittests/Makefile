DEVELHELP ?= 0
include ../Makefile.tests_common

USEMODULE += embunit

ifeq (, $(filter tests-%, $(MAKECMDGOALS)))
  # the $(dir) Makefile function leaves a trailing slash after the directory
  # name, therefore we use patsubst instead.
  UNIT_TESTS := $(patsubst %/Makefile,%,$(wildcard tests-*/Makefile))
else
  UNIT_TESTS := $(filter tests-%, $(MAKECMDGOALS))
endif

DISABLE_MODULE += auto_init auto_init_%

# boards using a preflashed bootloader require auto_init to
# automatically initialize stdio over USB. Without this, the bootloader
# management feature cannot be used (auto reset and auto reboot in bootloader
# mode)
FEATURES_BLACKLIST += bootloader_arduino
FEATURES_BLACKLIST += bootloader_nrfutil

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
