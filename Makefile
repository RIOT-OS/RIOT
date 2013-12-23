DIRS = $(RIOTCPU)/$(CPU) core drivers sys

.PHONY: all clean doc

include $(RIOTCPU)/$(CPU)/Makefile.include
include $(RIOTBOARD)/$(BOARD)/Makefile.include

all:
	mkdir -p $(BINDIR)
	@for i in $(DIRS) ; do "$(MAKE)" -C $$i ; done ;
	
clean:
	@for i in $(DIRS) ; do "$(MAKE)" -C $$i clean ; done ;
	-@if [ -d $(BINDIR) ] ; \
	then rm -rf $(BINDIR) ; \
	fi 

doc:
	make -BC doc/doxygen	

