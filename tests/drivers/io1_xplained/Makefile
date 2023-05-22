include ../Makefile.drivers_common

USEMODULE += io1_xplained
USEMODULE += ztimer_msec
# Hack to prevent some boards from bring in rtt and periph_timer backend
USEMODULE += ztimer_periph_timer

include $(RIOTBASE)/Makefile.include
