# set default port depending on operating system
# If the board does not provide a configuration, we assume that the serial port
# is some kind of USB to serial adapter and try to search for a device with a
# serial number matching the variable USB_UART_ID, falling back to
# DEBUG_ADAPTER_ID if USB_UART_ID is not given.
# If no serial number is given, then we pick the first USB tty we can find
OS := $(shell uname)
ifeq (,$(PORT))
  ifeq (,$(USB_UART_ID))
    USB_UART_ID := $(DEBUG_ADAPTER_ID)
  endif
  ifeq ($(OS),Linux)
    ifeq (,$(PORT_LINUX))
      # Search for a USB to serial adapter with the same USB serial number as the
      # debug adapter, fall back to searching for any USB TTY
      ifneq (,$(USB_UART_ID))
        # Match only exact serial number
        SERIAL_PATTERN := '^$(USB_UART_ID)$$'
      endif
      # if SERIAL_PATTERN is not set, we will get the first port of the first USB
      # to serial adapter
      PORT_LINUX := $(firstword $(shell $(RIOTBASE)/dist/tools/usb-serial/find-tty.sh $(SERIAL_PATTERN)))
    endif
    PORT := $(PORT_LINUX)
  else ifeq ($(OS),Darwin)
    ifeq (,$(PORT_DARWIN))
      ifneq (,$(USB_UART_ID))
        # Try to match with a USB serial number
        PORT_DARWIN := $(firstword $(sort $(wildcard /dev/tty.usbserial-$(USB_UART_ID)*)))
        # There may be other patterns which should be tried as well.
      endif
      ifeq (,$(PORT_DARWIN))
        # Look for any USB to serial adapter by default
        PORT_DARWIN := $(firstword $(sort $(wildcard /dev/tty.usbmodem* /dev/tty.usbserial-* /dev/tty.SLAB_USBtoUART*)))
      endif
    endif
    PORT := $(PORT_DARWIN)
  endif
endif
ifeq ($(PORT),)
  $(info Warning: no PORT set!)
endif
export BAUD ?= 115200

RIOT_TERMINAL ?= pyterm
ifeq ($(RIOT_TERMINAL),pyterm)
    TERMPROG  ?= $(RIOTBASE)/dist/tools/pyterm/pyterm
    TERMFLAGS ?= -p "$(PORT)" -b "$(BAUD)"
else ifeq ($(RIOT_TERMINAL),picocom)
    TERMPROG  ?= picocom
    TERMFLAGS ?= --nolock --imap lfcrlf --echo --baud "$(BAUD)" "$(PORT)"
endif
