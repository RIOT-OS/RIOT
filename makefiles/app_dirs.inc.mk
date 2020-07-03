.PHONY: info-applications info-applications-supported-boards

# fallback so empty RIOTBASE won't lead to "/examples/"
RIOTBASE ?= .

# 1. use wildcard to find Makefiles
# 2. use patsubst to drop trailing "/"
# 3. use patsubst to drop possible leading "./"
# 4. sort
APPLICATION_DIRS := $(sort $(patsubst ./%,%,$(patsubst %/,%,$(dir $(wildcard \
	$(RIOTBASE)/fuzzing/*/Makefile     \
	$(RIOTBASE)/bootloaders/*/Makefile \
	$(RIOTBASE)/examples/*/Makefile    \
	$(RIOTBASE)/tests/*/Makefile       \
	)))))

info-applications:
	@for dir in $(APPLICATION_DIRS); do echo $$dir; done

# All applications / board output of `info-boards-supported`.
info-applications-supported-boards:
	@for dir in $(APPLICATION_DIRS); do \
	  $(MAKE) --no-print-directory -C $${dir} info-boards-supported 2>/dev/null | xargs -n 1 echo $${dir}; \
	done
# BOARDS values from 'boards.inc.mk' to only evaluate it once
info-applications-supported-boards: export BOARDS ?=
