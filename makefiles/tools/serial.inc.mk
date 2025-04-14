# Select the most recently attached tty interface
ifeq (1,$(MOST_RECENT_PORT))
  ifeq (,$(TTY_SELECT_CMD))
    ifneq (,$(filter stdio_cdc_acm,$(USEMODULE)))
      TTY_SELECT_CMD := $(RIOTTOOLS)/usb-serial/ttys.py \
                        --most-recent \
                        --format path serial \
                        --model '$(BOARD)' --vendor 'RIOT-os\.org'
      # Allow matching board by RIOT serial
      ifneq (,$(SERIAL))
        TTY_SELECT_CMD += --serial "$(SERIAL)"
      endif
      # Allow matching the bootloader TTY as well, if not running RIOT but
      # but the bootloader
      ifneq (,$(PROG_TTY_BOARD_FILTER))
        TTY_SELECT_CMD += || $(RIOTTOOLS)/usb-serial/ttys.py \
                             --most-recent \
                             --format path serial \
                             $(PROG_TTY_BOARD_FILTER)
      endif
    else
      TTY_SELECT_CMD := $(RIOTTOOLS)/usb-serial/ttys.py \
                        --most-recent \
                        --format path serial \
                        $(TTY_BOARD_FILTER)
    endif
  endif
  TTY_DETECTED := $(shell $(TTY_SELECT_CMD) || echo 'no-tty-detected no-serial-detected')
  PORT_DETECTED := $(firstword $(TTY_DETECTED))
  PORT_SERIAL_DETECTED := $(lastword $(TTY_DETECTED))
  PORT ?= $(firstword $(TTY_DETECTED))
  ifeq (1,$(DEBUG_ADAPTER_ID_IS_TTY_SERIAL))
    DEBUG_ADAPTER_ID ?= $(PORT_SERIAL_DETECTED)
  endif
endif
# Otherwise, use as default the most commonly used ports on Linux and OSX
PORT_LINUX ?= /dev/ttyACM0
PORT_DARWIN ?= $(firstword $(sort $(wildcard /dev/tty.usbmodem*)))

# set default port depending on operating system
ifeq ($(OS),Linux)
  PORT ?= $(PORT_LINUX)
else ifeq ($(OS),Darwin)
  PORT ?= $(PORT_DARWIN)
endif

# Default PROG_DEV is the same as PORT
PROG_DEV ?= $(PORT)

export BAUD ?= 115200

ifneq (,$(filter stdio_rtt,$(USEMODULE)))
  ifeq (${PROGRAMMER},openocd)
    RIOT_TERMINAL ?= openocd-rtt
  else ifeq (${PROGRAMMER},jlink)
    RIOT_TERMINAL ?= jlink
  else ifeq (${RIOT_TERMINAL},)
    $(warning "Warning: No RIOT_TERMINAL set, but using stdio_rtt: The default terminal is likely not to work.")
  endif
endif

RIOT_TERMINAL ?= pyterm
ifeq ($(RIOT_TERMINAL),pyterm)
  PYTERMSESSION ?= $(shell date +%Y-%m-%d_%H.%M.%S)-$(APPLICATION)-$(BOARD)
  PYTERMLOGDIR ?= "/tmp/pyterm-$(USER)"
  TERMPROG  ?= $(RIOTTOOLS)/pyterm/pyterm
  TERMFLAGS ?= -p "$(PORT)" -b "$(BAUD)" -ln $(PYTERMLOGDIR) -rn "$(PYTERMSESSION)" $(PYTERMFLAGS)
else ifeq ($(RIOT_TERMINAL),socat)
  SOCAT_OUTPUT ?= -
  TERMPROG ?= $(RIOT_TERMINAL)
  TERMFLAGS ?= $(SOCAT_OUTPUT) open:$(PORT),b$(BAUD),echo=0,raw,cs8,parenb=0,cstopb=0
else ifeq ($(RIOT_TERMINAL),picocom)
  TERMPROG  ?= picocom
  TERMFLAGS ?= --nolock --imap lfcrlf --baud "$(BAUD)" "$(PORT)"
else ifeq ($(RIOT_TERMINAL),miniterm)
  # Check if miniterm.py is available in the path, if not use just miniterm
  # since new versions will only have miniterm and not miniterm.py
  ifeq (,$(shell command -v miniterm.py 2>/dev/null))
    TERMPROG ?= miniterm
  else
    TERMPROG ?= miniterm.py
  endif
  # The RIOT shell will still transmit back a CRLF, but at least with --eol LF
  # we avoid sending two lines on every "enter".
  TERMFLAGS ?= --eol LF "$(PORT)" "$(BAUD)" $(MINITERMFLAGS)
else ifeq ($(RIOT_TERMINAL),jlink)
  TERMPROG = $(RIOTTOOLS)/jlink/jlink.sh
  TERMFLAGS = term-rtt
else ifeq ($(RIOT_TERMINAL),semihosting)
  TERMPROG = $(DEBUGGER)
  TERMFLAGS = $(DEBUGGER_FLAGS)
  OPENOCD_DBG_EXTRA_CMD += -c 'arm semihosting enable'
  $(call target-export-variables,term cleanterm,OPENOCD_DBG_EXTRA_CMD)
else ifeq (${RIOT_TERMINAL},openocd-rtt)
  TERMENV = RAM_START_ADDR=${RAM_START_ADDR} RAM_LEN=${RAM_LEN}
  TERMPROG = $(RIOTTOOLS)/openocd/openocd.sh
  TERMFLAGS = term-rtt
else ifeq (${RIOT_TERMINAL},bootterm)
  TERMENV = BT_PORT_CRLF=1
  TERMPROG = $(RIOTTOOLS)/bootterm/bt
  TERMFLAGS = $(BOOTTERMFLAGS) -a -b $(BAUD) $(PORT)
  TERMDEPS += $(TERMPROG)
else ifeq (${RIOT_TERMINAL},native)
  TERMPROG ?= $(EXECUTABLE)
endif
