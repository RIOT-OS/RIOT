RIOTBASE=$(shell git rev-parse --show-toplevel)

# Generate list of quoted absolute include paths. Evaluated in riot.doxyfile.
export STRIP_FROM_INC_PATH_LIST=$(shell \
    git ls-tree -dr --full-tree --name-only HEAD core drivers sys |\
    grep '/include$$' |\
    sed 's/.*/\"$(subst /,\/,$(RIOTBASE))\/\0\"/')

# use lessc (http://lesscss.org/#using-less) for compiling CSS
# It can also be installed in ubuntu with the `node-less` package
ifeq (,$(LESSC))
  ifneq (,$(shell command -v lessc 2>/dev/null))
    LESSC=lessc
  else ifneq (,$(shell command -v lesscpy 2>/dev/null))
    LESSC=lesscpy
  else
    $(warning Did not find a LESS compiler, so I cannot build the CSS file in case you modified \
              the LESS file.)
  endif
endif

SED=sed
ifneq (,$(LESSC))
    UNAME := $(shell uname -s)
    ifeq ($(UNAME),Darwin)
        ifneq (,$(shell command -v gsed 2>/dev/null))
            SED=gsed
        else
            $(warning The version of sed that ships with macOS is very old, so you need gnu-sed. \
                      Please run 'brew install gnu-sed' and run 'make doc' again.)
       endif
    endif
endif

.PHONY: doc
doc: html

# by marking html as phony we force make to re-run Doxygen even if the directory exists.
.PHONY: html
html: src/css/riot.css src/changelog.md
	( cat riot.doxyfile ; echo "GENERATE_HTML = yes" ) | doxygen -

.PHONY: man
man: src/changelog.md
	( cat riot.doxyfile ; echo "GENERATE_MAN = yes" ) | doxygen -

ifneq (,$(LESSC))
src/css/riot.css: src/css/riot.less src/css/variables.less
	@$(LESSC) $< $@

src/css/variables.less: src/config.json
	@grep "^\s*\"@" $< | $(SED) -e 's/^\s*"//g' -e 's/":\s*"/: /g' \
	  -e 's/",\?$$/;/g' -e 's/\\"/"/g' > $@
endif

src/changelog.md: src/changelog.md.tmp ../../release-notes.txt
	@./generate-changelog.py $+ $@

.PHONY:
latex: src/changelog.md
	( cat riot.doxyfile ; echo "GENERATE_LATEX= yes" ) | doxygen -

clean:
	-@rm -rf latex man html doxygen_objdb_*.tmp doxygen_entrydb_*.tmp src/changelog.md
