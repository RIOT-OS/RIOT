FLASHFILE ?= $(BINFILE)

# -e: erase
STM32LOADER_FLAGS += -p $(PROG_DEV) -e -w $(FLASHFILE)

FLASHER ?= $(RIOTTOOLS)/stm32loader/stm32loader.py
FFLAGS ?= $(STM32LOADER_FLAGS)

include $(RIOTMAKE)/color.inc.mk

$(call echowarn,("Support for the stm32loader programmer is deprecated and"\
                 "will be removed after the 2027.04 release! Consider using"\
                 "openOCD instead."))
