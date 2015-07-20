.PHONY: doc
doc: html

# by marking html as phony we force make to re-run Doxygen even if the directory exists.
.PHONY: html
html:
	doxygen riot.doxyfile

clean:
	-@rm -rf latex man html doxygen_objdb_*.tmp
