DIST_FILES += bin/$(BOARD)/$(APPLICATION).a

ifneq (, $(filter check_bindist, $(MAKECMDGOALS)))
	include Makefile.distcheck
else
	DIRS+=$(BINARY_DIRS)
endif

bindist: all
	@mkdir -p bindist
	@for i in $(DIST_FILES) ; do \
		echo Copying $$i to bindist. ; \
		cp -a --parents $$i bindist ; \
		done
	@cp -a bin/$(BOARD)/$(APPLICATION).elf bindist

	@echo "BINDIST_RIOT_VERSION=$(RIOT_VERSION)" > bindist/Makefile.distcheck
	@echo "BINDIST_GIT_HEAD=$$(git --work-tree=$(RIOTBASE) describe)" >> bindist/Makefile.distcheck

prepare_check_bindist:
	@[ "$(BINDIST_RIOT_VERSION)" = "$(RIOT_VERSION)" ] || \
		echo "Warning! RIOT_VERSION doesn't match!"
	@[ "$(BINDIST_GIT_HEAD)" = "$$(git --work-tree=$(RIOTBASE) describe)" ] || \
		echo "Warning! git describe doesn't match!"

check_bindist: prepare_check_bindist all
	@test $(shell md5sum bin/$(BOARD)/$(APPLICATION).elf | cut -f1 -d\ ) \
		= $(shell md5sum $(APPLICATION).elf | cut -f1 -d\ ) \
		&& echo "bin/$(BOARD)/$(APPLICATION).elf matches $(APPLICATION).elf." \
		|| echo "bin/$(BOARD)/$(APPLICATION).elf and $(APPLICATION).elf don't match!"
