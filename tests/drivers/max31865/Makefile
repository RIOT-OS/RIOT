include ../Makefile.drivers_common

INCLUDES += -I$(APPDIR)

# include and auto-initialize all available sensors
USEMODULE += saul_default

USEMODULE += max31865
USEMODULE += ztimer
USEMODULE += ztimer_msec
USEMODULE += phydat

include $(RIOTBASE)/Makefile.include
