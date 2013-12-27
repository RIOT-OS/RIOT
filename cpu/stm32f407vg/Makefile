MODULE =cpu

INCLUDES += -I../include
 
DIRS = uart
ifneq (,$(findstring gpioint,$(USEMODULE)))
	DIRS += gpioint
endif
ifneq (,$(findstring mci,$(USEMODULE)))
	DIRS += mci
endif
ifneq (,$(findstring rtc,$(USEMODULE)))
	DIRS += rtc
endif

all: $(BINDIR)$(MODULE).a
	@for i in $(DIRS) ; do $(MAKE) -C $$i ; done ;

include $(RIOTBASE)/Makefile.base

clean::
	@for i in $(DIRS) ; do $(MAKE) -C $$i clean ; done ;

