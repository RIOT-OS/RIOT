RIOTBASE  ?= $(shell git rev-parse --show-toplevel)
RIOTMAKE  ?= $(RIOTBASE)/makefiles
RIOTTOOLS ?= $(RIOTBASE)/dist/tools


# Generate list of quoted absolute include paths. Evaluated in riot.doxyfile.
export STRIP_FROM_INC_PATH_LIST=$(shell \
    git ls-tree -dr --full-tree --name-only HEAD core drivers sys |\
    grep '/include$$' |\
    sed 's/.*/\"$(subst /,\/,$(RIOTBASE))\/\0\"/')

# use lessc (http://lesscss.org/#using-less) for compiling CSS
# It can also be installed in ubuntu with the `node-less` package
LESSC ?= lessc

# Extract the documentation type that is to be generated and export it for
# the doxygen Makefile
ifneq (,$(filter doc,$(MAKECMDGOALS)))
  DOCUMENTATION_FORMAT := html
else ifneq (,$(filter doc-%,$(MAKECMDGOALS)))
  DOCUMENTATION_FORMAT := $(patsubst doc-%,%,$(filter doc-%, $(MAKECMDGOALS)))
endif

# Check that the doxygen version is not too old to avoid
# certain bugs that were fixed in later revisions. Especially
# Debian-based distributions tend to have very old versions.
DOXYGEN_MIN_VERSION = 1.15.0

# Set the Doxygen binary if not already set
DOXYGEN ?= doxygen

# Strip the commit hash that is sometimes present after the
# version number.
DOXYGEN_CUR_VERSION = $(shell $(DOXYGEN) --version | cut -d ' ' -f1)

# for the `doc-ci` target, we can choose which Doxygen version should be built.
# If nothing has been chosen, select the minimum version.
# Valid options are `latest` and version numbers such as `1.13.2`.
DOXYGEN_VERSION ?= $(DOXYGEN_MIN_VERSION)

# include color echo macros
include $(RIOTMAKE)/utils/ansi.mk
include $(RIOTMAKE)/color.inc.mk

.PHONY: doc doc-man doc-latex
doc doc-man doc-latex: graphviz-check $(DOCUMENTATION_FORMAT) doxygen-version

# Check the Doxygen version and print a warning
.PHONY: doxygen-version
.NOTPARALLEL: doxygen-version
doxygen-version:
	@if [ "`{ echo "$(DOXYGEN_MIN_VERSION)"; echo "$(DOXYGEN_CUR_VERSION)"; } | \
	  sort -V | head -n1`" != "$(DOXYGEN_MIN_VERSION)" ]; then \
	  $(COLOR_ECHO) "$(COLOR_RED)Warning: Doxygen version $(DOXYGEN_CUR_VERSION) is too old." \
	  "It is recommended to use at least version $(DOXYGEN_MIN_VERSION)" \
	  "to avoid incorrectly formatted output.\nYou can use 'make doc-ci' to build" \
	  "the documentation with the required Doxygen version, which will be downloaded" \
	  "and (if required) compiled locally.$(COLOR_RESET)"; \
	fi

# GraphViz is a mandatory dependency to generate the Doxygen documentation for RIOT
.PHONY: graphviz-check
graphviz-check:
	@command -v dot >/dev/null 2>&1 || { \
	  $(COLOR_ECHO) "$(COLOR_RED)Error: GraphViz (dot) is not installed." \
	  "GraphViz is mandatory to generate the RIOT documentation.$(COLOR_RESET)"; \
	  exit 1; \
	}

.PHONY: doc-ci
doc-ci: $(RIOTTOOLS)/doxygen/Makefile
	@PKG_VERSION=$(DOXYGEN_VERSION) $(MAKE) -C $(RIOTTOOLS)/doxygen all --no-print-directory
	@$(MAKE) -BC $(CURDIR) DOXYGEN=$(RIOTTOOLS)/doxygen/doxygen doc

# by marking html as phony we force make to re-run Doxygen even if the directory exists.
.PHONY: html
html: src/changelog.md src/coc.md src/governance.md
	( cat riot.doxyfile ; echo "GENERATE_HTML = yes" ) | $(DOXYGEN) -
	@echo ""
	@echo "RIOT documentation successfully generated at file://$(RIOTBASE)/doc/doxygen/html/index.html"

.PHONY: check
check: src/changelog.md src/coc.md src/governance.md
	( cat riot.doxyfile) | $(DOXYGEN) -

.PHONY: man
man: src/changelog.md src/coc.md src/governance.md
	( cat riot.doxyfile ; echo "GENERATE_MAN = yes" ) | $(DOXYGEN) -
	@echo ""
	@echo "RIOT documentation successfully generated at file://$(RIOTBASE)/doc/doxygen/man/man3"

src/css/riot.css: src/css/riot.less src/css/variables.less
	@$(LESSC) $< $@

src/css/variables.less: src/config.json
	@grep "^\s*\"@" $< | sed -e 's/^\s*"//g' -e 's/":\s*"/: /g' \
	  -e 's/",\?$$/;/g' -e 's/\\"/"/g' > $@

src/changelog.md: src/changelog.md.tmp ../../release-notes.txt
	@./generate-changelog.py $+ $@

src/coc.md: ../../CODE_OF_CONDUCT.md
	awk 'NR == 1 {print $$0,"{#coc}"} NR > 1 {print $$0}' $< > $@

src/governance.md: ../../GOVERNANCE.md
	@sed 's/<!-- TOC start -->/\[TOC\]\n\0/' $< | sed '/<!-- TOC start -->/,/<!-- TOC end -->/d' > $@

.PHONY:
latex: src/changelog.md src/coc.md src/governance.md
	( cat riot.doxyfile ; echo "GENERATE_LATEX= yes" ) | $(DOXYGEN) -
	@echo ""
	@echo "RIOT documentation successfully generated at file://$(RIOTBASE)/doc/doxygen/latex/index.tex"

clean:
	-@rm -rf latex man html doxygen_objdb_*.tmp doxygen_entrydb_*.tmp src/changelog.md src/coc.md src/governance.md
	@$(MAKE) -C $(RIOTTOOLS)/doxygen clean
