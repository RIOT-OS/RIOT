BOARD ?= samr21-xpro
include ../Makefile.tests_common


# Normally all boards using `cortexm_common/ldscripts/cortexm.ld` linkerscript
# Only tested on these ones for the moment
BOARD_WHITELIST += iotlab-a8-m3
BOARD_WHITELIST += iotlab-m3
BOARD_WHITELIST += samr21-xpro

include $(RIOTBASE)/Makefile.include


# # # # # # # # # # # # # # # # # # #
# Compile time tests for ROM_OFFSET #
# # # # # # # # # # # # # # # # # # #

COMPILE_TESTS = test-elffile-overflow

all: compile-tests

compile-tests: $(COMPILE_TESTS)
.PHONY: compile-tests $(COMPILE_TESTS)


# test-elffile-overflow depends on $(BINFILE) to prevent:
# * ROM_LEN to be passed to $(ELFFILE) generation
# * dummy error message of wc not finding the .bin file (ELFFILE is not enough)
test-elffile-overflow: $(BINFILE) _test-elffile-overflow-runtest

.PHONY: _test-elffile-overflow-runtest
_test-elffile-overflow-runtest: ROM_LEN=$(firstword $(shell wc -c $(BINFILE)))-1+$(if $(ROM_OFFSET),$(ROM_OFFSET),0)
_test-elffile-overflow-runtest: $(BINFILE)
	$(Q)echo -n "Test rom offset 1 byte overflow detection: "
	$(Q)\
	{ $(_LINK) -o /dev/null  2>&1 | grep -q "region \`rom' overflowed by 1 byte" ; } \
	  && echo [OK] || { echo [ERROR] Compilation should have failed >&2; exit 1; }
