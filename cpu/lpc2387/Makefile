MODULE =cpu

include $(RIOTCPU)/$(CPU)/Makefile.include

DIRS = $(RIOTCPU)/arm_common $(RIOTCPU)/lpc_common periph

ifneq (,$(filter gpioint,$(USEMODULE)))
	DIRS += gpioint
endif
ifneq (,$(filter mci,$(USEMODULE)))
	DIRS += mci
endif
ifneq (,$(filter rtc,$(USEMODULE)))
	DIRS += rtc
endif
ifneq (,$(filter i2c,$(USEMODULE)))
	DIRS += i2c
endif

all: $(BINDIR)$(MODULE).a
	@for i in $(DIRS) ; do "$(MAKE)" -C $$i || exit 1; done ;

include $(RIOTBASE)/Makefile.base
