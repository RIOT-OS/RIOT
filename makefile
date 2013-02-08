DIRS = cpu core drivers sys 

all:
	@for i in $(DIRS) ; do $(MAKE) -C $$i ; done ;
	
clean:
	@for i in $(DIRS) ; do $(MAKE) -C $$i clean ; done ;

doc:
	make -BC doc/doxygen	
