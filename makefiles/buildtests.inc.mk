.PHONY: buildtest

BUILDTEST_MAKE_REDIRECT ?= >/dev/null 2>&1

ifeq ($(BUILD_IN_DOCKER),1)
buildtest: ..in-docker-container
else
buildtest:
	@ \
	RESULT=true ; \
	for board in $(BOARDS); do \
		if BOARD=$${board} $(MAKE) check-toolchain-supported > /dev/null 2>&1; then \
			$(COLOR_ECHO) -n "Building for $$board ... " ; \
			BOARD=$${board} RIOT_CI_BUILD=1 RIOT_VERSION_OVERRIDE=buildtest \
				$(MAKE) clean all -j $(NPROC) $(BUILDTEST_MAKE_REDIRECT); \
			RES=$$? ; \
			if [ $$RES -eq 0 ]; then \
				$(COLOR_ECHO) "$(call c_green,success.)" ; \
			else \
				$(COLOR_ECHO) "$(call c_red,failed!)" ; \
				RESULT=false ; \
			fi ; \
			BOARD=$${board} $(MAKE) clean-intermediates >/dev/null 2>&1 || true; \
		fi; \
	done ; \
	$${RESULT}
endif # BUILD_IN_DOCKER
