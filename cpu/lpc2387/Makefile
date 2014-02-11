MODULE =cpu

include $(RIOTCPU)/$(CPU)/Makefile.include

DIRS = $(RIOTCPU)/arm_common $(RIOTCPU)/lpc_common
ifneq (,$(findstring gpioint,$(USEMODULE)))
	DIRS += gpioint
endif
ifneq (,$(findstring mci,$(USEMODULE)))
	DIRS += mci
endif
ifneq (,$(findstring rtc,$(USEMODULE)))
	DIRS += rtc
endif
ifneq (,$(findstring i2c,$(USEMODULE)))
	DIRS += i2c
endif

all: $(BINDIR)$(MODULE).a
	@for i in $(DIRS) ; do "$(MAKE)" -C $$i ; done ;

include $(RIOTBASE)/Makefile.base

clean::
	@for i in $(DIRS) ; do "$(MAKE)" -C $$i clean ; done ;
