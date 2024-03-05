MODULE =cpu

include $(CPUDIR)/Makefile.include

DIRS = $(RIOTCPU)/arm7_common periph

ifneq (,$(filter mci,$(USEMODULE)))
	DIRS += mci
endif
ifneq (,$(filter i2c,$(USEMODULE)))
	DIRS += i2c
endif

include $(RIOTBASE)/Makefile.base
