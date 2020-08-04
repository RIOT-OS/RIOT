# OpenOCD settings for Mulle programmer board.
# Try to determine which OpenOCD config file we should use based on the
# programmer board serial number.

# Fall back to PROGRAMMER_SERIAL for backwards compatibility
DEBUG_ADAPTER_ID ?= $(PROGRAMMER_SERIAL)

ifneq (,$(DEBUG_ADAPTER_ID))
  # Makefile-way of comparing numbers, using lexicographical sorting since we
  # don't have any arithmetic comparisons.
  # Programmers with serial 100 -- 148 are version 0.60
  # Programmers with serial 301 -- 330 are version 0.70
  ifeq "100" "$(word 1, $(sort 100 $(DEBUG_ADAPTER_ID)))"
    # >= 100
    ifneq "149" "$(word 1, $(sort 149 $(DEBUG_ADAPTER_ID)))"
      # < 149
      PROGRAMMER_VERSION = 0.60
    else
      # >= 149
      PROGRAMMER_VERSION = 0.70
    endif
  endif
endif
# Default to version 0.70 programmer
PROGRAMMER_VERSION ?= 0.70

OPENOCD_ADAPTER_INIT ?= -f '$(RIOTBASE)/boards/mulle/dist/openocd/mulle-programmer-$(PROGRAMMER_VERSION).cfg'

# Default to SWD
OPENOCD_TRANSPORT ?= swd

# Add serial matching command, only if DEBUG_ADAPTER_ID was specified
ifneq (,$(DEBUG_ADAPTER_ID))
  OPENOCD_ADAPTER_INIT += -c 'ftdi_serial $(DEBUG_ADAPTER_ID)'
endif
