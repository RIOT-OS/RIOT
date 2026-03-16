include ../Makefile.drivers_common

USEMODULE += bitfield
USEMODULE += shell
USEMODULE += ztimer
USEMODULE += ztimer_msec

# Select any of the following variants: amg8831, amg8832, amg8833, amg8834,
# amg8851, amg8852, amg8853 or amg8854.
AMG88XX_VARIANT ?= amg8833

USEMODULE += $(AMG88XX_VARIANT)

# Uncomment the following lines to enable the display of the sensor data on a
# graphical display.
# USEMODULE += st77xx
# USEMODULE += disp_dev

include $(RIOTBASE)/Makefile.include
