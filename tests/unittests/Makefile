DEVELHELP ?= 0

# Allow including this Makefile from other apps, as done by
# tests/build_system/external_unittests
UNIT_TESTS_DIR ?= $(CURDIR)
include $(UNIT_TESTS_DIR)/../Makefile.tests_common

USEMODULE += embunit

# Search for unit tests both in-tree and, if configured, out-of-tree
UNIT_TEST_SEARCH_DIRS := $(CURDIR) $(EXTERNAL_UNITTEST_DIRS)
# Canonicalize test search dirs: Add a single solidus at the end:
UNIT_TEST_SEARCH_DIRS := $(addsuffix /,$(abspath $(UNIT_TEST_SEARCH_DIRS)))

ifeq (,$(UNIT_TESTS))
  ifeq (, $(filter tests-%, $(MAKECMDGOALS)))
    # collect all unit test makefiles in all unit test search dirs
    UNIT_TESTS_MAKEFILES := $(wildcard $(addsuffix tests-*/Makefile,$(UNIT_TEST_SEARCH_DIRS)))
    # $(dir) would leave a solidus at the end, using patsub instead
    UNIT_TESTS := $(patsubst %/Makefile,%,$(UNIT_TESTS_MAKEFILES))
    # $(notdir) gives the last component in path, despite its name
    UNIT_TESTS := $(notdir $(UNIT_TESTS))
  else
    UNIT_TESTS := $(filter tests-%, $(MAKECMDGOALS))
  endif
endif

# in case unit tests were provided by name, we need to locate them
ifeq (,$(UNIT_TESTS_MAKEFILES))
  # generate "$searchdir/$testname/Makefile" for every possible combination of
  # $searchdir and $testname
  UNIT_TESTS_MAKEFILES := $(addsuffix /Makefile,$(UNIT_TESTS))
  UNIT_TESTS_MAKEFILES := $(foreach testdir,$(UNIT_TEST_SEARCH_DIRS),$(addprefix $(testdir),$(UNIT_TESTS_MAKEFILES)))
  # reduce that list to files that actually exist
  UNIT_TESTS_MAKEFILES := $(wildcard $(UNIT_TESTS_MAKEFILES))
endif

ifeq (llvm,$(TOOLCHAIN))
  # the floating point exception bug is more likely to trigger when build
  # with LLVM, so we just disable LLVM on native as a work around
  TEST_ON_CI_BLACKLIST += native32 native64
endif

DISABLE_MODULE += auto_init auto_init_%

# boards using stdio via CDC ACM require auto_init to automatically
# initialize stdio over USB.
FEATURES_BLACKLIST += highlevel_stdio

# extract the test dirs from the unit test makefiles
UNIT_TEST_DIRS := $(dir $(UNIT_TESTS_MAKEFILES))

# each unit test dir needs to be build as module
DIRS += $(UNIT_TEST_DIRS)

# Pull in `Makefile.include`s from the test suites:
-include $(addsuffix Makefile.include,$(UNIT_TEST_DIRS))
BASELIBS += $(UNIT_TESTS:%=%.module)

INCLUDES += -I$(RIOTBASE)/tests/unittests/common

# some tests need more stack
CFLAGS += -DTHREAD_STACKSIZE_MAIN=THREAD_STACKSIZE_LARGE
CFLAGS += -DCONFIG_CORE_EXIT_WITH_MAIN=1

# for these boards, enable asan (Address Sanitizer)
ASAN_BOARDS ?= native32 native64
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

# Hack: If GNRC is not used, still provide access to sock_types.h to allow
# building nanocoap
ifeq (,$(filter gnrc_sock,$(USEMODULE)))
  CFLAGS +=-I$(RIOTBASE)/sys/net/gnrc/sock/include
endif
