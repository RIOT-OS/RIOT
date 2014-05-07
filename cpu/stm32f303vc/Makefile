
# define the module that is build
MODULE = cpu

# add a list of subdirectories, that should also be build
DIRS = periph

.PHONY: all $(STM_LIB_NAME)

# when building the cpu module make sure, the STM std perip lib will also be build
all: $(STM_LIB_NAME) $(CORTEX_COMMON) $(BINDIR)$(MODULE).a
	@for i in $(DIRS) ; do $(MAKE) -C $$i ; done ;

$(STM_LIB_NAME):
	@$(MAKE) -C $(STM_LIB_BASE)

$(CORTEX_COMMON):
	@$(MAKE) -C $(CORTEX_COMMON)

include $(RIOTBASE)/Makefile.base

clean::
	@for i in $(DIRS) ; do $(MAKE) -C $$i clean ; done ;
	@$(MAKE) -C $(STM_LIB_BASE) clean
