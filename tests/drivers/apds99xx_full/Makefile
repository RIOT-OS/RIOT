
MODULE_DEFS_NEEDED += \
  apds9900 \
  apds9901 \
  apds9930 \
  apds9950 \
  apds9960 \
  #
include ../Makefile.drivers_common

DRIVER ?= apds9960

USEMODULE += $(DRIVER)
USEMODULE += apds99xx_full
USEMODULE += core_thread_flags

include $(RIOTBASE)/Makefile.include
