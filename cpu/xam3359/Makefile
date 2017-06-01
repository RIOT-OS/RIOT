
# define the module that is build
MODULE = cpu

# add a list of subdirectories, that should also be build
DIRS = periph starterware


# when building the CPU module make sure, the STM std peripheral lib will also be build
all: $(BINDIR)$(MODULE).a
	@for i in $(DIRS) ; do $(MAKE) -C $$i ; done ;


include $(RIOTBASE)/Makefile.base

clean::
	@for i in $(DIRS) ; do $(MAKE) -C $$i clean ; done ;

