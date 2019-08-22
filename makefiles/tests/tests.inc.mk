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

# this target only makes sense if an ELFFILE is actually created, thus guard by
# RIOTNOLINK="".
ifeq (,$(RIOTNOLINK))
test-input-hash: $(TESTS) $(ELFFILE) $(TEST_EXTRA_FILES)
	sha1sum $^ > $(BINDIR)/test-input-hash.sha1
else
test-input-hash:
	true
endif
