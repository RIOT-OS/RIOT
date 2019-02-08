RIOTBASE=$(shell git rev-parse --show-toplevel)

include $(RIOTBASE)/makefiles/buildout.inc.mk

# Generate list of quoted absolute include paths. Evaluated in riot.doxyfile.
export STRIP_FROM_INC_PATH_LIST=$(shell \
    git ls-tree -dr --full-tree --name-only HEAD core drivers sys |\
    grep '/include$$' |\
    sed 's/.*/\"$(subst /,\/,$(RIOTBASE))\/\0\"/')

# use lessc (http://lesscss.org/#using-less) for compiling CSS
# It can also be installed in ubuntu with the `node-less` package
LESSC ?= $(shell command -v lessc 2>/dev/null)

DOXYGEN ?= doxygen -

DOXY_ERROR_PIPE = ($(DOXYGEN) 3>&2 2>&1 1>&3 3>&- | tee $(LOGS_DIR)/doxygen.log | \
		   sed -nf warning_patterns.sed | sort | uniq -c) 3>&2 2>&1 1>&3 3>&-

.PHONY: doc
doc: html

# by marking html as phony we force make to re-run Doxygen even if the directory exists.
.PHONY: html
html: src/css/riot.css src/changelog.md | $(LOGS_DIR)
	( cat riot.doxyfile ; echo "GENERATE_HTML = yes" ) | $(DOXY_ERROR_PIPE)

.PHONY: man
man: src/changelog.md | $(LOGS_DIR)
	( cat riot.doxyfile ; echo "GENERATE_MAN = yes" ) | $(DOXY_ERROR_PIPE)

ifneq (,$(LESSC))
src/css/riot.css: src/css/riot.less src/css/variables.less
	@$(LESSC) $< $@

src/css/variables.less: src/config.json
	@grep "^\s*\"@" $< | sed -e 's/^\s*"//g' -e 's/":\s*"/: /g' \
	  -e 's/",\?$$/;/g' -e 's/\\"/"/g' > $@
endif

src/changelog.md: src/changelog.md.tmp ../../release-notes.txt
	@./generate-changelog.py $+ $@

.PHONY:
latex: src/changelog.md | $(LOGS_DIR)
	( cat riot.doxyfile ; echo "GENERATE_LATEX= yes" ) | $(DOXY_ERROR_PIPE)

clean:
	-@rm -rf latex man html doxygen_objdb_*.tmp doxygen_entrydb_*.tmp src/changelog.md
