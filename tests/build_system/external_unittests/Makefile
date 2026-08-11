
MODULE_DEFS_NEEDED += \
  xtimer \
  ztimer64_msec \
  ztimer64_sec \
  ztimer64_usec \
  ztimer_msec \
  ztimer_sec \
  ztimer_usec \
  ztimer_xtimer_compat \
  #
# Run this only on native64 / native32, as this rather tests the build system
# than the code actually run.

BOARDS_SUPPORTED := native32 native64

EXTERNAL_UNITTEST_DIRS := $(CURDIR)/external_tests_dir

# Build upon tests/unittests:
RIOTBASE ?= $(CURDIR)/../../..
UNIT_TESTS_DIR := $(CURDIR)/../../unittests
include ../../unittests/Makefile
