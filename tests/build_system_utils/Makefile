BOARD_WHITELIST = native

include ../Makefile.tests_common
include $(RIOTBASE)/Makefile.include


# Test utils commands

ok_message = $(COLOR_ECHO) "$(COLOR_GREEN) [OK] $1 $(COLOR_RESET)"
fail_message = $(COLOR_ECHO) "$(COLOR_RED) [FAIL] $1 $(COLOR_RESET)"

define command_should_fail
$1 2>/dev/null && { $(call fail_message,Command '$1' should have failed but did not) >&2; $1; exit 1; } || $(call ok_message,$2)
endef

define command_should_succeed
($1 && $(call ok_message,$2) )  || { $(call fail_message,Command '$1' failed) >&2; $1; exit 1; }
endef

MAKEFILES_UTILS = $(RIOTMAKE)/utils

# Abstract away the most common test procedure:
# In these rule, the firs prerequisite MUST be the .mk file where the test
# is defined

test-%-negative:
	$(Q)$(call command_should_fail,"$(MAKE)" -C $(MAKEFILES_UTILS) -f $< $@-negative,$* (negative))

test-%:
	$(Q)$(call command_should_succeed,"$(MAKE)" -C $(MAKEFILES_UTILS) -f $< $@,$*)

# Tests will be run both in the host machine and in `docker`
all: build-system-utils-tests

# tests for 'ensure_value', 'assert_etc'
TEST_CHECKS = 	test-ensure_value test-assert test-assert_not \
				test-ensure_value-negative test-assert-negative \
				test-assert_not-negative

COMPILE_TESTS += $(TEST_CHECKS)

$(TEST_CHECKS): $(MAKEFILES_UTILS)/test-checks.mk

# tests for variable helpers

TEST_VARS = test-exported-variables test-memoized-variables

$(TEST_VARS): $(MAKEFILES_UTILS)/test-variables.mk

COMPILE_TESTS += $(TEST_VARS)

# test for string equality. They depend on having a working assert and assert_not
TEST_STREQ = test-streq
COMPILE_TESTS += $(TEST_STREQ)

$(TEST_STREQ): $(MAKEFILES_UTILS)/test-strings.mk $(TEST_CHECKS)

# Continue testing assertions. These require a working streq.
TEST_ASSERT_EQ = test-assert_eq test-assert_eq-negative
COMPILE_TESTS += $(TEST_ASSERT_EQ)

$(TEST_ASSERT_EQ): $(MAKEFILES_UTILS)/test-checks.mk $(TEST_STREQ)

TEST_CHECKS_ALL = $(TEST_CHECKS) $(TEST_ASSERT_EQ)

# Now that we know string equality assertions work, we can run the rest of
# the tests
TEST_STRINGS = test-strnot test-rest test-concat test-intercal
COMPILE_TESTS += $(TEST_STRINGS)

$(TEST_STRINGS): $(MAKEFILES_UTILS)/test-strings.mk $(TEST_CHECKS_ALL)

# Path manipulation (this requires string functions to work)
TEST_PATHS = test-relpath
ifeq ($(OS),Linux)
  # Only test against coreutils in linux
  TEST_PATHS += test-relpath-against-sys
endif

COMPILE_TESTS += $(TEST_PATHS)

$(TEST_PATHS): $(MAKEFILES_UTILS)/test-paths.mk $(TEST_STRINGS)

.PHONY: build-system-utils-tests
# $(COMPILE_TESTS) should be phony but that conflicts with the implicit rule
$(COMPILE_TESTS): FORCE

build-system-utils-tests: $(COMPILE_TESTS)
	@true
