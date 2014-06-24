.all:

.PHONY: all doc docclean welcome

all: welcome
	@echo ""
	@exit 1

doc:
	"$(MAKE)" -BC doc/doxygen

docclean:
	"$(MAKE)" -BC doc/doxygen clean

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
