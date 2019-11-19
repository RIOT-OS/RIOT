# set default port depending on operating system
ifeq ($(OS),Linux)
  PORT ?= $(call ensure_value,$(PORT_LINUX),No port set)
else ifeq ($(OS),Darwin)
  PORT ?= $(call ensure_value,$(PORT_DARWIN),No port set)
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
  TERMFLAGS ?= --eol LF "$(PORT)" "$(BAUD)"
endif
