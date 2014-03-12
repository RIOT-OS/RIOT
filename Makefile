DIRS = $(RIOTCPU)/$(CPU) core drivers sys

.PHONY: all clean doc

all:
	mkdir -p $(BINDIR)
	@for i in $(DIRS) ; do "$(MAKE)" -C $$i || exit 1; done ;

clean:
	@for i in $(DIRS) ; do "$(MAKE)" -C $$i clean || exit 1; done ;
	-@if [ -d $(BINDIR) ] ; \
	then rm -rf $(BINDIR) ; \
	fi

doc:
	"$(MAKE)" -BC doc/doxygen

docclean:
	"$(MAKE)" -BC doc/doxygen clean
