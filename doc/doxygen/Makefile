RIOTBASE=$(shell git rev-parse --show-toplevel)
# Generate list of quoted absolute include paths. Evaluated in riot.doxyfile.
export STRIP_FROM_INC_PATH_LIST=$(shell \
    git ls-tree -dr --full-tree --name-only HEAD core drivers sys |\
    grep '/include$$' |\
    sed 's/.*/\"$(subst /,\/,${RIOTBASE})\/\0\"/')

.PHONY: doc
doc: html

# by marking html as phony we force make to re-run Doxygen even if the directory exists.
.PHONY: html
html: src/css/riot.css
	( cat riot.doxyfile ; echo "GENERATE_HTML = yes" ) | doxygen -

.PHONY: man
man:
	( cat riot.doxyfile ; echo "GENERATE_MAN = yes" ) | doxygen -

ifneq (,$(shell which lessc))
# use lessc (http://lesscss.org/#using-less) for compiling CSS
src/css/riot.css: src/css/riot.less src/css/variables.less
	@lessc $< $@

src/css/variables.less: src/config.json
	@grep "^\s*\"@" $< | sed -e 's/^\s*"//g' -e 's/":\s*"/: /g' \
	  -e 's/",\?$$/;/g' -e 's/\\"/"/g' > $@
endif

.PHONY:
latex:
	( cat riot.doxyfile ; echo "GENERATE_LATEX= yes" ) | doxygen -

clean:
	-@rm -rf latex man html doxygen_objdb_*.tmp doxygen_entrydb_*.tmp
