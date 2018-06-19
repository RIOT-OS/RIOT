# Minimal makefile for Sphinx documentation
#

# You can set these variables from the command line.
SPHINXOPTS    = -j auto
SPHINXBUILD   = sphinx-build
SPHINXPROJ    = RIOTOS
SOURCEDIR     = .
BUILDDIR      = _build

# Put it first so that "make" without argument is like "make help".
help:
	$(SPHINXBUILD) -M help "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS)

.PHONY: help

clean:
	rm -rf _build
	rm -rf breathe
	(cd doxygen && make clean)

.PHONY: clean

breathe/grouplist.rst: doxygen/xml/index.xml
	breathe-apidoc -g group -o breathe doxygen/xml
	rm  breathe/group/group__boards__*
	rm  breathe/group/group__cpu__*

doxygen/xml/index.xml:
	(cd doxygen && make xml)

# Catch-all target: route all unknown targets to Sphinx using the new
# "make mode" option.  $(O) is meant as a shortcut for $(SPHINXOPTS).
html: breathe/grouplist.rst
	$(SPHINXBUILD) -M $@ "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS)

