# Run this only on native64 / native, as this rather tests the build system
# than the code actually run.

BOARD_WHITELIST := native native64

EXTERNAL_UNITTEST_DIRS := $(CURDIR)/external_tests_dir

# Build upon tests/unittests:
RIOTBASE ?= $(CURDIR)/../../..
UNIT_TESTS_DIR := $(CURDIR)/../../unittests
include ../../unittests/Makefile
