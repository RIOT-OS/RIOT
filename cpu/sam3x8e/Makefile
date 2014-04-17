
# define the module that is build
MODULE = cpu

# add a list of subdirectories, that should also be build
DIRS = periph

.PHONY: all $(CORTEX_COMMON)

# when building the CPU module make sure, the STM std peripheral lib will also be build
all: $(CORTEX_COMMON) $(BINDIR)$(MODULE).a
	@for i in $(DIRS) ; do $(MAKE) -C $$i ; done ;

$(CORTEX_COMMON):
	@$(MAKE) -C $(CORTEX_COMMON)

include $(RIOTBASE)/Makefile.base

clean::
	@for i in $(DIRS) ; do $(MAKE) -C $$i clean ; done ;
	@$(MAKE) -C $(CORTEX_COMMON) clean
