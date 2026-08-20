# This file contains support for gcov-based code coverage reporting on native.
#
# Usage (from tests/unittests or any native application):
#
#   make coverage [UNIT_TESTS=tests-<name>] [COVERAGE_OUTPUT=<dir>]
#
# Prerequisites:
#   lcov and genhtml must be installed on the host.

include $(RIOTMAKE)/utils/ansi.mk
include $(RIOTMAKE)/color.inc.mk

COVERAGE_OUTPUT ?= coverage

CFLAGS_COVERAGE  = -fprofile-arcs -ftest-coverage -O0 -fno-inline -D__GCOV__
LINKFLAGS_COVERAGE = --coverage

# Flags are injected by the 'coverage' target before invoking 'all'
all-coverage: CFLAGS    += $(CFLAGS_COVERAGE)
all-coverage: LINKFLAGS += $(LINKFLAGS_COVERAGE)
all-coverage: all

.PHONY: coverage coverage/run coverage/collect coverage/html coverage/clean

DOCKER_RIOTBASE ?= /data/riotbuild/riotbase

# Top-level target: build -> run -> report.
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
	@echo "Collecting coverage data..."
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
	@echo "Generating HTML report in $(COVERAGE_OUTPUT)/..."
	@command -v genhtml >/dev/null 2>&1 || \
	  (echo "genhtml not found. Install it with: apt install lcov / brew install lcov"; exit 1)
	$(Q)genhtml $(COVERAGE_OUTPUT).info \
	            --output-directory $(COVERAGE_OUTPUT) \
	            --title "RIOT unit-test coverage ($(BOARD))" \
	            --ignore-errors source \
	            --quiet

# Run the native binary so .gcda files are written on exit.
# Stale .gcda files from previous runs cause timestamp conflicts, so they are
# removed first.  Test failures produce a non-zero exit code but should not
# abort coverage collection, so the exit status is intentionally ignored here.
coverage/run: $(ELFFILE)
	@echo "Running $(ELFFILE) to collect coverage data..."
	$(Q)find $(BINDIR) -name '*.gcda' -delete
	$(Q)$(ELFFILE); true

# Remove generated artefacts (.gcda files and report)
coverage/clean:
	$(Q)find $(BINDIR) -name '*.gcda' -delete
	$(Q)find $(BINDIR) -name '*.gcno' -delete
	$(Q)rm -rf $(COVERAGE_OUTPUT) $(COVERAGE_OUTPUT).info
