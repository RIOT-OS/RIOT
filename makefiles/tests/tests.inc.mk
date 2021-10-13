# Export TESTRUNNER_RESET_AFTER_TERM only for the test target. This allows for
# it to be accessed through the environment from python test script.
# This is currently needed only by `examples/%/tests` and should be removed in
# the future since `make reset` after `term` is not a valid synch method across
# all platforms.
$(call target-export-variables,test test-as-root test-with-config,TESTRUNNER_RESET_AFTER_TERM)

.PHONY: test test/available
TESTS ?= $(foreach file,$(wildcard $(APPDIR)/tests/*[^~]),\
                        $(shell test -f $(file) -a -x $(file) && echo $(file)))

# "make test" calls "make term" which would implicitly build it's dependencies,
# but that increases the time "make test" needs to get ready. That can cause
# problems ("make term" missing some lines) as terminal startup is not properly
# sychronized, but depends on a static timeout (TESTRUNNER_START_DELAY).
# Murdock builds the term dependencies before running "make test" to circumvent
# this. In order to make local builds behave similar, add the term deps here.
# See #11762.
TEST_DEPS += $(TERMDEPS)

test: $(TEST_DEPS)
	$(Q) for t in $(TESTS); do \
		$$t || exit 1; \
	done

test/available:
	$(Q)test -n "$(strip $(TESTS))"

# Tests that require root privileges
.PHONY: test-as-root test-as-root/available
TESTS_AS_ROOT ?= $(foreach file,$(wildcard $(APPDIR)/tests-as-root/*[^~]),\
                    $(shell test -f $(file) -a -x $(file) && echo $(file)))

test-as-root: $(TEST_DEPS)
	$(Q) for t in $(TESTS_AS_ROOT); do \
		sudo -E PYTHONPATH=$(PYTHONPATH) $$t || exit 1; \
	done

test-as-root/available:
	$(Q)test -n "$(strip $(TESTS_AS_ROOT))"

# Tests that require specific configuration
.PHONY: test-with-config test-with-config/available test-with-config/check-config
# Scripts for tests that require specific configuration (with automatic
# configuration checks filtered out)
TESTS_WITH_CONFIG ?= $(filter-out $(APPDIR)/tests-with-config/check-config%,\
  $(foreach file,$(wildcard $(APPDIR)/tests-with-config/*[^~]),\
     $(shell test -f $(file) -a -x $(file) && echo $(file)))\
)
# Scripts that provide an automatic configuration check
# for tests with specific configuration
CHECK_CONFIG ?= $(foreach file,$(wildcard $(APPDIR)/tests-with-config/check-config*[^~]),\
                  $(shell test -f $(file) -a -x $(file) && echo $(file)))

test-with-config: test-with-config/check-config $(TEST_DEPS)
	$(Q) for t in $(TESTS_WITH_CONFIG); do \
		$$t || exit 1; \
	done

# run the automatic configuration check
test-with-config/check-config:
	$(Q)test -z "$(strip $(CHECK_CONFIG))" || \
		${COLOR_ECHO} -n "${COLOR_RED}"; \
		for t in $(CHECK_CONFIG) empty; do \
			test $$t = "empty" && break; \
			$$t || \
			(echo "System configuration for" \
			"tests is not available, check $${t#$${RIOTBASE}/} failed." \
			"${COLOR_RESET}"; \
				exit 1) || exit 1; \
		done; \
		${COLOR_ECHO} -n "${COLOR_RESET}"

# this target only makes sense if an ELFFILE is actually created, thus guard by
# RIOTNOLINK="".
ifeq (,$(RIOTNOLINK))
test-input-hash: $(TESTS) $(TESTS_WITH_CONFIG) $(TESTS_AS_ROOT) $(ELFFILE) $(TEST_EXTRA_FILES)
	sha1sum $^ > $(BINDIR)/test-input-hash.sha1
else
test-input-hash:
	true
endif
