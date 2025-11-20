.PHONY: info-applications info-applications-supported-boards

# fallback so empty RIOTBASE won't lead to "/examples/"
RIOTBASE ?= .

APPS_BASE_DIRS = bootloaders examples fuzzing tests
TOOLS_BASE_DIRS = dist/tools

# 1. recursively find Makefiles
# 2. take parent folders
# 3. exclude build artifacts (below bin/)
# 4. exclude false positives from app_dirs.blacklist
# 5. sort
APPLICATION_DIRS := $(shell find $(APPS_BASE_DIRS) -name Makefile -type f | \
    xargs dirname | \
    grep -vF "/bin/" | \
    grep -vFf $(RIOTBASE)/makefiles/app_dirs.blacklist | \
    sort | uniq)

# used for `make claen` and `make distclean`
TOOLS_DIRS := $(shell find $(TOOLS_BASE_DIRS) -mindepth 2 -maxdepth 2 -name Makefile -type f | \
    xargs dirname | \
    grep -vFf $(RIOTBASE)/makefiles/app_dirs.blacklist | \
    sort | uniq)

info-applications:
	@for dir in $(APPLICATION_DIRS); do echo $$dir; done

# All applications / board output of `info-boards-supported`.
info-applications-supported-boards:
	@for dir in $(APPLICATION_DIRS); do \
	  $(MAKE) --no-print-directory -C $${dir} info-boards-supported 2>/dev/null | xargs -n 1 echo $${dir}; \
	done
# BOARDS values from 'boards.inc.mk' to only evaluate it once
info-applications-supported-boards: export BOARDS ?=
