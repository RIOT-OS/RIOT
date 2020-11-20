# Use as default the most commonly used ports on Linux and OSX
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

RIOT_TERMINAL ?= pyterm
ifeq ($(RIOT_TERMINAL),pyterm)
  TERMPROG  ?= $(RIOTTOOLS)/pyterm/pyterm
  TERMFLAGS ?= -p "$(PORT)" -b "$(BAUD)" $(PYTERMFLAGS)
else ifeq ($(RIOT_TERMINAL),socat)
  SOCAT_OUTPUT ?= -
  TERMPROG ?= $(RIOT_TERMINAL)
  TERMFLAGS ?= $(SOCAT_OUTPUT) open:$(PORT),b$(BAUD),echo=0,raw
else ifeq ($(RIOT_TERMINAL),picocom)
  TERMPROG  ?= picocom
  TERMFLAGS ?= --nolock --imap lfcrlf --baud "$(BAUD)" "$(PORT)"
else ifeq ($(RIOT_TERMINAL),miniterm)
  TERMPROG  ?= miniterm.py
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
endif
