ifeq (, $(__RIOTBUILD_FLAG))
  all: welcome
	@echo ""
	@exit 1
else
  all:
	mkdir -p $(BINDIR)
	@for i in $(DIRS) ; do "$(MAKE)" -C $$i || exit 1; done ;
endif

DIRS = $(RIOTCPU)/$(CPU) core drivers sys

ifneq (,$(filter embunit,$(USEMODULE)))
	DIRS += tests/unittests/embunit/embUnit
endif

ifneq (,$(filter embunit_textui,$(USEMODULE)))
	DIRS += tests/unittests/embunit/textui
endif

.PHONY: all clean doc docclean welcome

clean:
	@for i in $(DIRS) ; do "$(MAKE)" -C $$i clean || exit 1; done ;
	-@if [ -d $(BINDIR) ] ; \
	then rm -rf $(BINDIR) ; \
	fi

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
