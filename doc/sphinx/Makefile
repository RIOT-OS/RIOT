# Minimal makefile for Sphinx documentation
#

# You can set these variables from the command line.
SPHINXOPTS    ?= -j 8
SPHINXBUILD   ?= sphinx-build
SOURCEDIR     ?= .
BUILDDIR      ?= _build

STUBSCRIPT    ?= gen_stubs.py
STUBGEN       ?= python $(STUBSCRIPT)

.PHONY: help html xml sphinx-clean doxy-clean gen-clean clean view sphinx-prereq

html xml pickle help: $(SPHINX_PREREQ)
	@$(SPHINXBUILD) -M $@ "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)

INDICES ?= generated/file-index.rst generated/group-index.rst generated/page-index.rst

DOXY_DIR = ../doxygen/
DOXY_XML_DIR = $(DOXY_DIR)/xml
SPHINX_PREREQ = $(DOXY_XML_DIR) $(INDICES)

# Catch-all target: route all unknown targets to Sphinx using the new
# "make mode" option.  $(O) is meant as a shortcut for $(SPHINXOPTS).

sphinx-prereq: $(SPHINX_PREREQ)

generated/%-index.rst: $(DOXY_XML_DIR) $(STUBSCRIPT)
	$(STUBGEN) --$(*F)s $< $(dir $@)

$(DOXY_XML_DIR): $(DOXY_DIR)/Makefile
	$(MAKE) -C $(DOXY_DIR) xml
	touch $@

# The file "doxy-xml" will contain the location of the xml directory
doxy-xml: RIOT/doc/doxygen/xml
	$(file >$@,$^)

sphinx-clean:
	@$(SPHINXBUILD) -M clean "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)

doxy-clean:
	rm -rf $(DOXY_XML_DIR)
	rm -rf doxy-xml

gen-clean:
	rm -rf generated

clean: sphinx-clean doxy-clean gen-clean

view: html
	xdg-open $(BUILDDIR)/html/index.html
