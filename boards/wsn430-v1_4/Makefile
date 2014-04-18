MODULE = $(BOARD)_base

DIRS = $(RIOTBOARD)/wsn430-common

INCLUDES += -I$(RIOTBASE)/drivers/cc2420/include \
			-I$(RIOTBASE)/sys/net/include

all: $(BINDIR)$(MODULE).a
	@for i in $(DIRS) ; do "$(MAKE)" -C $$i || exit 1; done ;

include $(RIOTBASE)/Makefile.base
