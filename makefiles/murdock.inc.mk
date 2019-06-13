#
# This file contains helper targets used by the CI.
#
#
# This target will run "make test" on the CI cluster.
#
# In order to work, these requirements must be fulfilled:
# - DWQ_REPO and DWQ_COMMIT are set correctly
# - the user has set up autossh & proper authentication for connecting to the CI
# (intended to be used by CI only for now)
#
# By default, $(FLASHFILE) is copied to the test worker along with the job.
# If the test needs any extra files, they can be added to the TEST_EXTRA_FILES
# variable. They will also be sent to the test worker.
# Note: *any TEST_EXTRA_FILES has to reside in ${BINDIR}*.
test-murdock:
	cd $(RIOTBASE) && \
		./.murdock test_job \
		$$(realpath --relative-to $(RIOTBASE) $(APPDIR)) \
		"$(BOARD):$(TOOLCHAIN)" \
		$(FLASHFILE) $(TEST_EXTRA_FILES)

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
