DEVELHELP ?= 0
include ../Makefile.tests_common

USEMODULE += embunit

ifeq (, $(UNIT_TESTS))
  ifeq (, $(filter tests-%, $(MAKECMDGOALS)))
    # the $(dir) Makefile function leaves a trailing slash after the directory
    # name, therefore we use patsubst instead.
    UNIT_TESTS := $(patsubst %/Makefile,%,$(wildcard tests-*/Makefile))
  else
    UNIT_TESTS := $(filter tests-%, $(MAKECMDGOALS))
  endif
endif

ifeq (llvm,$(TOOLCHAIN))
  # the floating point exception bug is more likely to trigger when build
  # with LLVM, so we just disable LLVM on native as a work around
  TEST_ON_CI_BLACKLIST += native native64
endif

DISABLE_MODULE += auto_init auto_init_%

# boards using stdio via CDC ACM require auto_init to automatically
# initialize stdio over USB.
FEATURES_BLACKLIST += highlevel_stdio

# Pull in `Makefile.include`s from the test suites:
-include $(UNIT_TESTS:%=$(RIOTBASE)/tests/unittests/%/Makefile.include)

DIRS += $(UNIT_TESTS)
BASELIBS += $(UNIT_TESTS:%=%.module)

INCLUDES += -I$(RIOTBASE)/tests/unittests/common

# some tests need more stack
CFLAGS += -DTHREAD_STACKSIZE_MAIN=THREAD_STACKSIZE_LARGE
CFLAGS += -DCONFIG_CORE_EXIT_WITH_MAIN=1

# for these boards, enable asan (Address Sanitizer)
ASAN_BOARDS ?= native native64
ifneq (, $(filter $(ASAN_BOARDS), $(BOARD)))
  CFLAGS += $(CFLAGS_ASAN)
  LINKFLAGS += $(LINKFLAGS_ASAN)
endif

include $(RIOTBASE)/Makefile.include

.PHONY: $(UNIT_TESTS)

all:

info-unittests:
	@echo $(UNIT_TESTS)

$(UNIT_TESTS): all

ifeq (, $(UNIT_TESTS))
  CFLAGS += -DNO_TEST_SUITES
  $(warning There was no test suite specified!)
else
  CFLAGS += -DTEST_SUITES='$(subst $() $(),$(comma),$(UNIT_TESTS:tests-%=%))'
endif
