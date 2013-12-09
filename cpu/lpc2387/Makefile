MODULE =cpu

INCLUDES = -I../include -Idrivers/include -I../arm_common/include -I../../sys/lib -I../../drivers/include -I../../core/include -I../../sys/include -I../../hal/include -I../../.. 
DIRS =
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

