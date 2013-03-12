MODULE = cpu

INCLUDES = -I../include -I$(RIOTBASE)/core/include
DIRS =
ifneq (,$(findstring rtc,$(USEMODULE)))
	DIRS += rtc
endif

all: $(BINDIR)$(MODULE).a
	@for i in $(DIRS) ; do $(MAKE) -C $$i ; done ;

include $(RIOTBASE)/Makefile.base

clean::
	@for i in $(DIRS) ; do $(MAKE) -C $$i clean ; done ;

