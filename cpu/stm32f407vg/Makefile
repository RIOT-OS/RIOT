MODULE =cpu

INCLUDES += -I../include -I../stm32_common/STM32F4xx_StdPeriph_Driver/inc

USEMODULE += stm32f4xxstdperi
 
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

all: $(BINDIR)$(MODULE).a
	@for i in $(DIRS) ; do $(MAKE) -C $$i ; done ;

include $(RIOTBASE)/Makefile.base

clean::
	@for i in $(DIRS) ; do $(MAKE) -C $$i clean ; done ;

