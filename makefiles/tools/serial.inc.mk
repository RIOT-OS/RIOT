# set default port depending on operating system
OS := $(shell uname)
ifeq ($(OS),Linux)
  PORT ?= $(PORT_LINUX)
else ifeq ($(OS),Darwin)
  PORT ?= $(PORT_DARWIN)
endif
ifeq ($(PORT),)
  $(info Warning: no PORT set!)
endif
export PORT

export BAUD ?= 115200

RIOT_TERMINAL ?= pyterm
ifeq ($(RIOT_TERMINAL),pyterm)
  export TERMPROG  ?= $(RIOTTOOLS)/pyterm/pyterm
  export TERMFLAGS ?= -p "$(PORT)" -b "$(BAUD)"
else ifeq ($(RIOT_TERMINAL),picocom)
  export TERMPROG  ?= picocom
  export TERMFLAGS ?= --nolock --imap lfcrlf --echo --baud "$(BAUD)" "$(PORT)"
else ifeq ($(RIOT_TERMINAL),miniterm.py)
  export TERMPROG  ?= miniterm.py
  # The RIOT shell will still transmit back a CRLF, but at least with --eol LF
  # we avoid sending two lines on every "enter".
  export TERMFLAGS ?= --eol LF "$(PORT)" "$(BAUD)"
endif
