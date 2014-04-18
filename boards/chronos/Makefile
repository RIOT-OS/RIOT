MODULE =$(BOARD)_base

INCLUDES += -I$(RIOTBOARD)/$(BOARD)/drivers/include
DIRS = drivers

all: $(BINDIR)$(MODULE).a
	@for i in $(DIRS) ; do "$(MAKE)" -C $$i || exit 1; done ;

include $(RIOTBASE)/Makefile.base
