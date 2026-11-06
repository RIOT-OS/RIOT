# Run this only on native64 / native32, as this rather tests the build system
# than the code actually run.

BOARDS_SUPPORTED := native32 native64

EXTERNAL_UNITTEST_DIRS := $(CURDIR)/external_tests_dir

# Build upon tests/unittests:
RIOTBASE ?= $(CURDIR)/../../..
UNIT_TESTS_DIR := $(CURDIR)/../../unittests
include ../../unittests/Makefile
