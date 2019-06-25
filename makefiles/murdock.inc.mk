#
# This file contains helper targets used by the CI.
#

# (HACK) get actual flash binary from FFLAGS if not defined.
FLASHFILE ?= $(filter $(HEXFILE) $(ELFFILE:.elf=.bin) $(ELFFILE),$(FFLAGS))

#
# This target will run "make test" on the CI cluster.
#
# In order to work, these requirements must be fulfilled:
# - DWQ_REPO and DWQ_COMMIT are set correctly
# - the user has set up autossh & proper authentication for connecting to the CI
# (intended to be used by CI only for now)
test-murdock:
	cd $(RIOTBASE) && \
		./.murdock test_job \
		$$(realpath --relative-to $(RIOTBASE) $(APPDIR)) \
		"$(BOARD):$(TOOLCHAIN)" \
		$(FLASHFILE)


# Control running tests on boards during the CI tests.
#
# Testing is enabled if all the following conditions are met:
#
#  * the board is whitelisted
#  * the board has enough memory and the executable is being linked
#
# TEST_ON_CI_WHITELIST can be empty, a board list or 'all'

TEST_ON_CI_WHITELIST ?=
TEST_ON_BOARD_ENABLED ?= $(filter $(TEST_ON_CI_WHITELIST:all=%),$(BOARD))

TEST_ON_CI_ENABLED ?= $(if $(RIOTNOLINK),,$(TEST_ON_BOARD_ENABLED))

.PHONY: test-on-ci-enabled
test-on-ci-enabled:
	$(Q)test -n "$(TEST_ON_CI_ENABLED)"


# create $(BINDIR)/.test file only if the test is enabled for this board
.PHONY: $(BINDIR)/.test
link: $(BINDIR)/.test
$(BINDIR)/.test: $(filter clean, $(MAKECMDGOALS))
ifneq (,$(TEST_ON_CI_ENABLED))
	$(Q)mkdir -p $(BINDIR)
	$(Q)touch $@
else
	$(Q)rm -f $@
endif
