MODULE =cpu

include $(RIOTCPU)/$(CPU)/Makefile.include

DIRS = $(RIOTCPU)/arm7_common periph

ifneq (,$(filter gpioint,$(USEMODULE)))
	DIRS += gpioint
endif
ifneq (,$(filter mci,$(USEMODULE)))
	DIRS += mci
endif
ifneq (,$(filter i2c,$(USEMODULE)))
	DIRS += i2c
endif

include $(RIOTBASE)/Makefile.base
