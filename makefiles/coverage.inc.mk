# This file contains support for gcov-based code coverage reporting on native.
#
# Usage (from tests/unittests or any native application):
#
#   make coverage [UNIT_TESTS=tests-<name>] [COVERAGE_OUTPUT=<dir>]
#
# Prerequisites:
#   - Without Docker: lcov and genhtml must be installed on the host, and the
#     host gcov must be compatible with the GCC version used to compile.
#   - With BUILD_IN_DOCKER=1: only Docker is required.  lcov is installed
#     automatically inside the riotbuild container.

COVERAGE_OUTPUT ?= coverage

CFLAGS_COVERAGE  = -fprofile-arcs -ftest-coverage -O0 -fno-inline -D__GCOV__
LINKFLAGS_COVERAGE = --coverage

# Flags are injected by the 'coverage' target before invoking 'all'
all-coverage: CFLAGS    += $(CFLAGS_COVERAGE)
all-coverage: LINKFLAGS += $(LINKFLAGS_COVERAGE)
all-coverage: all

.PHONY: coverage coverage/run coverage/collect coverage/html \
        coverage/report-docker coverage/clean

DOCKER_RIOTBASE ?= /data/riotbuild/riotbase

# Top-level target: build → run → report.
#
# When BUILD_IN_DOCKER=1 the build + run steps execute inside the riotbuild
# container (via ..in-docker-container).  DOCKER_MAKECMDGOALS is overridden so
# Docker only runs 'all-coverage coverage/run'; the reporting step is handled
# afterwards by coverage/report-docker which installs lcov inside the same
# riotbuild container and generates the HTML report.
ifeq ($(BUILD_IN_DOCKER),1)
  ifneq (,$(filter coverage,$(MAKECMDGOALS)))
    override DOCKER_MAKECMDGOALS := all-coverage coverage/run
  endif

# Absolute paths of build artefacts as seen inside the reporting container.
# RIOTBASE is mounted at DOCKER_RIOTBASE (same path the riotbuild container
# uses), so the relative layout is identical.
_COV_DOCKER_BINDIR  := $(DOCKER_RIOTBASE)/$(patsubst $(RIOTBASE)/%,%,$(BINDIR))
_COV_DOCKER_APPDIR  := $(DOCKER_RIOTBASE)/$(patsubst $(RIOTBASE)/%,%,$(CURDIR))
_COV_DOCKER_OUTINFO := $(_COV_DOCKER_APPDIR)/$(COVERAGE_OUTPUT).info
_COV_DOCKER_OUTDIR  := $(_COV_DOCKER_APPDIR)/$(COVERAGE_OUTPUT)

coverage: ..in-docker-container coverage/report-docker
	@$(COLOR_ECHO) "$(COLOR_GREEN)Coverage report: $(COVERAGE_OUTPUT)/index.html$(COLOR_RESET)"

# Run lcov and genhtml inside the riotbuild container.
# RIOTBASE is bind-mounted at DOCKER_RIOTBASE (the same path used during the
# build), so all embedded source paths resolve correctly for genhtml.
coverage/report-docker:
	@$(COLOR_ECHO) "$(COLOR_GREEN)Collecting coverage and generating HTML report (in Docker)...$(COLOR_RESET)"
	$(Q)$(DOCKER) run --rm --platform linux/amd64 \
	    -v '$(RIOTBASE):$(DOCKER_RIOTBASE)' \
	    $(DOCKER_IMAGE) \
	    bash -c "\
	        apt-get update -qq \
	        && apt-get install -qq -y --no-install-recommends lcov \
	        && lcov --capture \
	                 --directory $(_COV_DOCKER_BINDIR) \
	                 --base-directory $(DOCKER_RIOTBASE) \
	                 --output-file $(_COV_DOCKER_OUTINFO) \
	                 --exclude '*/tests/*' \
	                 --exclude '/usr/*' \
	                 --ignore-errors source \
	                 --quiet \
	        && genhtml $(_COV_DOCKER_OUTINFO) \
	                    --output-directory $(_COV_DOCKER_OUTDIR) \
	                    --title 'RIOT unit-test coverage ($(BOARD))' \
	                    --ignore-errors source \
	                    --quiet"

else

coverage: all-coverage coverage/run coverage/collect coverage/html
	@$(COLOR_ECHO) "$(COLOR_GREEN)Coverage report: $(COVERAGE_OUTPUT)/index.html$(COLOR_RESET)"

# On macOS the system gcov is LLVM-based and cannot read GCC-format .gcda
# files.  Prefer a GCC gcov from Homebrew if one is available.
GCOV_TOOL ?= $(firstword $(wildcard \
    /opt/homebrew/bin/gcov \
    /usr/local/bin/gcov \
    $(addsuffix /gcov,$(wildcard /opt/homebrew/Cellar/gcc/*/bin)) \
    $(addsuffix /gcov-15,$(wildcard /opt/homebrew/Cellar/gcc/*/bin)) \
    $(addsuffix /gcov-14,$(wildcard /opt/homebrew/Cellar/gcc/*/bin)) \
    $(addsuffix /gcov-13,$(wildcard /opt/homebrew/Cellar/gcc/*/bin)) \
    $(addsuffix /gcov-12,$(wildcard /opt/homebrew/Cellar/gcc/*/bin)) \
    $(addsuffix /gcov-11,$(wildcard /opt/homebrew/Cellar/gcc/*/bin)) \
    ) gcov)

GCOV_TOOL_FLAG = $(if $(filter-out gcov,$(GCOV_TOOL)),--gcov-tool $(GCOV_TOOL),)

# Collect coverage data from .gcda / .gcno artefacts.
# --ignore-errors source     : skip source files not found on the host
coverage/collect:
	@$(COLOR_ECHO) "$(COLOR_GREEN)Collecting coverage data...$(COLOR_RESET)"
	@command -v lcov >/dev/null 2>&1 || \
	  (echo "lcov not found. Install it with: apt install lcov / brew install lcov"; exit 1)
	$(Q)lcov --capture \
	         --directory $(BINDIR) \
	         --base-directory $(RIOTBASE) \
	         --output-file $(COVERAGE_OUTPUT).info \
	         --exclude '*/tests/*' \
	         --exclude '/usr/*' \
	         --ignore-errors mismatch,unsupported,source,version \
	         $(GCOV_TOOL_FLAG) \
	         --quiet

# Render HTML report
coverage/html:
	@$(COLOR_ECHO) "$(COLOR_GREEN)Generating HTML report in $(COVERAGE_OUTPUT)/...$(COLOR_RESET)"
	@command -v genhtml >/dev/null 2>&1 || \
	  (echo "genhtml not found. Install it with: apt install lcov / brew install lcov"; exit 1)
	$(Q)genhtml $(COVERAGE_OUTPUT).info \
	            --output-directory $(COVERAGE_OUTPUT) \
	            --title "RIOT unit-test coverage ($(BOARD))" \
	            --ignore-errors source \
	            --quiet

endif

# Run the native binary so .gcda files are written on exit.
# Stale .gcda files from previous runs cause timestamp conflicts, so they are
# removed first.  Test failures produce a non-zero exit code but should not
# abort coverage collection, so the exit status is intentionally ignored here.
coverage/run: $(ELFFILE)
	@$(COLOR_ECHO) "$(COLOR_GREEN)Running $(ELFFILE) to collect coverage data...$(COLOR_RESET)"
	$(Q)find $(BINDIR) -name '*.gcda' -delete
	$(Q)$(ELFFILE); true

# Remove generated artefacts (.gcda files and report)
coverage/clean:
	$(Q)find $(BINDIR) -name '*.gcda' -delete
	$(Q)find $(BINDIR) -name '*.gcno' -delete
	$(Q)rm -rf $(COVERAGE_OUTPUT) $(COVERAGE_OUTPUT).info
