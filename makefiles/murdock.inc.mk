#
# This file contains helper targets used by the CI.
#

# (HACK) get actual flash binary from FFLAGS.
FLASHFILE:=$(filter $(HEXFILE) $(ELFFILE:.elf=.bin) $(ELFFILE),$(FFLAGS))

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

# don't whitelist tests if there's no binary
ifeq (1,$(RIOTNOLINK))
  TEST_ON_CI_WHITELIST:=
endif

# create $(BINDIR)/.test file only if BOARD is in $(TEST_ON_CI_WHITELIST)
.PHONY: $(BINDIR)/.test
link: $(BINDIR)/.test
$(BINDIR)/.test: $(filter clean, $(MAKECMDGOALS))
ifneq (,$(filter $(BOARD) all, $(TEST_ON_CI_WHITELIST)))
	$(Q)mkdir -p $(BINDIR)
	$(Q)touch $@
else
	$(Q)rm -f $@
endif
