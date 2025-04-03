.PHONY: buildtest buildtest-indocker

BUILDTEST_MAKE_REDIRECT ?= >/dev/null 2>&1

buildtest:
	@ \
	RESULT=true ; \
	for board in $(BOARDS); do \
		if BOARD=$${board} $(MAKE) check-toolchain-supported > /dev/null 2>&1; then \
			$(COLOR_ECHO) -n "Building for $$board ... " ; \
			env --unset=MAKEFLAGS BOARD=$${board} RIOT_CI_BUILD=1 \
				$(MAKE) clean all -j $(NPROC) $(BUILDTEST_MAKE_REDIRECT); \
			RES=$$? ; \
			if [ $$RES -eq 0 ]; then \
				$(COLOR_ECHO) "$(COLOR_GREEN)success.$(COLOR_RESET)" ; \
			else \
				$(COLOR_ECHO) "$(COLOR_RED)failed!$(COLOR_RESET)" ; \
				RESULT=false ; \
			fi ; \
			env --unset=MAKEFLAGS BOARD=$${board} $(MAKE) clean-intermediates >/dev/null 2>&1 || true; \
		fi; \
	done ; \
	$${RESULT}

# Define 'buildtest-indocker' completely executed inside the container.
# It prevents starting one container per compilation which is slower but it
# could hide errors where the host toolchain would be used
ifeq ($(BUILD_IN_DOCKER),1)
  buildtest-indocker: ..in-docker-container
else
ifeq ($(INSIDE_DOCKER),1)
  buildtest-indocker: buildtest
else
  buildtest-indocker:
	$(error $@ must be run with `BUILD_IN_DOCKER=1`)
endif # INSIDE_DOCKER
endif # BUILD_IN_DOCKER
