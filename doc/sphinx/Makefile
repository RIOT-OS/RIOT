# Minimal makefile for Sphinx documentation
#

# You can set these variables from the command line.
SPHINXOPTS    = -c "$(CONFIGDIR)" -j auto
SPHINXBUILD   = sphinx-build
SPHINXPROJ    = RIOTOS
SOURCEDIR     = ..
CONFIGDIR     = $(SOURCEDIR)/sphinx/source
BUILDDIR      = $(SOURCEDIR)/sphinx/build

# Put it first so that "make" without argument is like "make help".
help:
	@$(SPHINXBUILD) -M help -c "$(CONFIGDIR)" "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)

.PHONY: help Makefile

# Catch-all target: route all unknown targets to Sphinx using the new
# "make mode" option.  $(O) is meant as a shortcut for $(SPHINXOPTS).
%: Makefile
	@$(SPHINXBUILD) -M $@ "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)
