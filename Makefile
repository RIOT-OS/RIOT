.all:

.PHONY: all doc doc-man doc-latex docclean welcome

all: welcome
	@echo ""
	@exit 1

doc:
	"$(MAKE)" -BC doc/doxygen

doc-man:
	"$(MAKE)" -BC doc/doxygen man

doc-latex:
	"$(MAKE)" -BC doc/doxygen latex

docclean:
	"$(MAKE)" -BC doc/doxygen clean

clean:
	@echo "Cleaning all build products for the current board"
	@find ./pkg/ ./examples/ ./tests/ -maxdepth 2 -mindepth 2 -type f -name Makefile -execdir "${MAKE}" clean ';'

distclean: docclean
	@echo "Cleaning all build products"
	@find ./pkg/ ./examples/ ./tests/ -maxdepth 2 -mindepth 2 -type f -name Makefile -execdir "${MAKE}" distclean ';'

welcome:
	@echo "Welcome to RIOT - The friendly OS for IoT!"
	@echo ""
	@echo "You executed 'make' from the base directory."
	@echo "You should run 'make' in your application's directory instead."
	@echo ""
	@echo "Please see our Quick Start Guide at:"
	@echo "    https://github.com/RIOT-OS/RIOT/wiki/Quick-Start-Guide"
	@echo "Or ask questions on our mailing list:"
	@echo "    users@riot-os.org (http://lists.riot-os.org/mailman/listinfo/users)"
