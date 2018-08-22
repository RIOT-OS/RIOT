RIOTBASE=$(shell git rev-parse --show-toplevel)
# Generate list of quoted absolute include paths. Evaluated in riot.doxyfile.
export STRIP_FROM_INC_PATH_LIST=$(shell \
    git ls-tree -dr --full-tree --name-only HEAD core drivers sys |\
    grep '/include$$' |\
    sed 's/.*/\"$(subst /,\/,$(RIOTBASE))\/\0\"/')

# use lessc (http://lesscss.org/#using-less) for compiling CSS, alternatively
# fall back to lesscpy (https://github.com/lesscpy/lesscpy)
ifeq (,$(LESSC))
  ifneq (,$(shell command -v lessc 2>/dev/null))
    LESSC=lessc
  else ifneq (,$(shell command -v lesscpy 2>/dev/null))
    LESSC=lesscpy
  endif
endif

.PHONY: doc
doc: html

# by marking html as phony we force make to re-run Doxygen even if the directory exists.
.PHONY: html
html: git-lfs src/css/riot.css src/changelog.md
	( cat riot.doxyfile ; echo "GENERATE_HTML = yes" ) | doxygen -

.PHONY: man
man: git-lfs src/changelog.md
	( cat riot.doxyfile ; echo "GENERATE_MAN = yes" ) | doxygen -

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
latex: git-lfs src/changelog.md
	( cat riot.doxyfile ; echo "GENERATE_LATEX= yes" ) | doxygen -

RED=\033[0;31m
NC=\033[0m

git-lfs: git-lfs-check git-lfs-install

git-lfs-check:
	@git lfs --version > /dev/null 2>&1 || (echo -e "$(RED)Please install git lfs.$(NC)" && exit 1)

git-lfs-install:
	@git config --list | grep lfs > /dev/null || git lfs install --local

clean:
	-@rm -rf latex man html doxygen_objdb_*.tmp doxygen_entrydb_*.tmp src/changelog.md
