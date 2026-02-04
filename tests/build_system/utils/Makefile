BOARD_WHITELIST = native32 native64

include ../Makefile.build_system_common
include $(RIOTBASE)/Makefile.include


# Test utils commands
define command_should_fail
$1 2>/dev/null && { echo "Command '$1' should have failed but did not" >&2; $1; exit 1; } || true
endef

define command_should_succeed
$1 || { echo "Command '$1' failed" >&2; $1; exit 1; }
endef


MAKEFILES_UTILS = $(RIOTMAKE)/utils

COMPILE_TESTS += test-ensure_value test-ensure_value-negative
COMPILE_TESTS += test-exported-variables
COMPILE_TESTS += test-memoized-variables
COMPILE_TESTS += test-lowercase
COMPILE_TESTS += test-uppercase
COMPILE_TESTS += test-uppercase_and_underscore
COMPILE_TESTS += test-version_is_greater
COMPILE_TESTS += test-version_is_greater_or_equal
COMPILE_TESTS += test-max_number

# Tests will be run both in the host machine and in `docker`
all: build-system-utils-tests

build-system-utils-tests: $(COMPILE_TESTS)
.PHONY: build-system-utils-tests $(COMPILE_TESTS)


# tests for 'ensure_value'
test-ensure_value:
	$(Q)$(call command_should_succeed,"$(MAKE)" -C $(MAKEFILES_UTILS) -f test-checks.mk test-ensure_value)

test-ensure_value-negative:
	$(Q)$(call command_should_fail,"$(MAKE)" -C $(MAKEFILES_UTILS) -f test-checks.mk test-ensure_value-negative)

test-exported-variables:
	$(Q)$(call command_should_succeed,"$(MAKE)" -C $(MAKEFILES_UTILS) -f test-variables.mk test-exported-variables)

test-memoized-variables:
	$(Q)$(call command_should_succeed,"$(MAKE)" -C $(MAKEFILES_UTILS) -f test-variables.mk test-memoized-variables)

test-lowercase:
	$(Q)$(call command_should_succeed,"$(MAKE)" -C $(MAKEFILES_UTILS) -f test-strings.mk test-lowercase)

test-uppercase:
	$(Q)$(call command_should_succeed,"$(MAKE)" -C $(MAKEFILES_UTILS) -f test-strings.mk test-uppercase)

test-uppercase_and_underscore:
	$(Q)$(call command_should_succeed,"$(MAKE)" -C $(MAKEFILES_UTILS) -f test-strings.mk test-uppercase_and_underscore)

test-version_is_greater:
	$(Q)$(call command_should_succeed,"$(MAKE)" -C $(MAKEFILES_UTILS) -f test-strings.mk test-version_is_greater)

test-version_is_greater_or_equal:
	$(Q)$(call command_should_succeed,"$(MAKE)" -C $(MAKEFILES_UTILS) -f test-strings.mk test-version_is_greater_or_equal)

test-max_number:
	$(Q)$(call command_should_succeed,"$(MAKE)" -C $(MAKEFILES_UTILS) -f test-strings.mk test-max_number)
