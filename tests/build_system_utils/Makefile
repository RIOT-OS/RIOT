BOARD_WHITELIST = native

include ../Makefile.tests_common
include $(RIOTBASE)/Makefile.include


# Test utils commands
define command_should_fail
$1 2>/dev/null && { echo "Command '$1' should have failed but did not" >&2; $1; exit 1; } || true
endef

define command_should_succeed
$1 || { echo "Command '$1' failed" >&2; $1; exit 1; }
endef


MAKEFILES_UTILS = $(RIOTMAKE)/utils

COMPILE_TESTS = test-ensure_value test-ensure_value-negative

# Tests will be run both in the host machine and in `docker`
all: build-system-utils-tests

build-system-utils-tests: $(COMPILE_TESTS)
.PHONY: build-system-utils-tests $(COMPILE_TESTS)


# tests for 'ensure_value'
test-ensure_value:
	$(Q)$(call command_should_succeed,"$(MAKE)" -C $(MAKEFILES_UTILS) -f test-checks.mk test-ensure_value)

test-ensure_value-negative:
	$(Q)$(call command_should_fail,"$(MAKE)" -C $(MAKEFILES_UTILS) -f test-checks.mk test-ensure_value-negative)
