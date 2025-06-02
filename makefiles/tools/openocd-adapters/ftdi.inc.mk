# Select default FTDI debug adapter board
OPENOCD_FTDI_ADAPTER ?= tigard

# FTDI debug adapter interface
OPENOCD_ADAPTER_INIT ?= -c 'source [find interface/ftdi/$(OPENOCD_FTDI_ADAPTER).cfg]'

# default to SWD
OPENOCD_TRANSPORT ?= swd

# Add serial matching command, only if DEBUG_ADAPTER_ID was specified
ifneq (,$(DEBUG_ADAPTER_ID))
  OPENOCD_ADAPTER_INIT += -c 'ftdi_serial $(DEBUG_ADAPTER_ID)'
endif

ifeq (swd, $(OPENOCD_TRANSPORT))
  ifeq (tigard, $(OPENOCD_FTDI_ADAPTER))
    # Add dummy SWD_EN signal
    OPENOCD_ADAPTER_INIT += -c 'ftdi layout_signal SWD_EN -data 0'
  endif
endif

ifeq (tigard,$(OPENOCD_FTDI_ADAPTER))
  # The Tigard provides both (bitbanging) SWD/JTAG debugger and UART adapter.
  # When freshly connected, the Tigard will provide to UART interface, of
  # which the first will be used for UART and the second be reconfigured for
  # bit-banging JTAG/SWD. We assume the UART of the Tigard is used for serial
  # by default.
  TTY_BOARD_FILTER ?= --model 'Tigard V1' --iface-num 0

  # The debug adapter and the UART adapter are the same device, so they have
  # the same serial number
  DEBUG_ADAPTER_ID_IS_TTY_SERIAL ?= 1

  # If the user provided a programmer serial, we also use that to detect the
  # TTY.
  ifneq (,$(SERIAL))
    TTY_SELECT_CMD += --serial "$(SERIAL)"
  endif
endif
