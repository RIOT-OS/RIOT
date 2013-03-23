DIRS = $(RIOTCPU) core drivers sys 

all:
	mkdir -p $(BINDIR)
	@for i in $(DIRS) ; do $(MAKE) -C $$i ; done ;
	
clean:
	@for i in $(DIRS) ; do $(MAKE) -C $$i clean ; done ;
	@if [ -d $(BINDIR) ] ; \
	then rmdir --ignore-fail-on-non-empty $(BINDIR) ; \
	fi 

doc:
	make -BC doc/doxygen	
