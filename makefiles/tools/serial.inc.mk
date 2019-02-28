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
  TERMPROG  ?= $(RIOTTOOLS)/pyterm/pyterm
  TERMFLAGS ?= -p "$(PORT)" -b "$(BAUD)"
else ifeq ($(RIOT_TERMINAL),rlwrap)
  TERMPROG  ?= $(RIOT_TERMINAL)
  RLWRAP_PROMPT ?= -pPurple -S 'RIOT $$ '
  RLWRAP_FLAGS ?= -a -C RIOT-$(BOARD)
  TERMFLAGS  ?= $(RLWRAP_PROMPT) $(RLWRAP_FLAGS) $(RAWTERMPROG) $(RAWTERMFLAGS)
else ifeq ($(RIOT_TERMINAL),picocom)
  TERMPROG  ?= picocom
  TERMFLAGS ?= --nolock --imap lfcrlf --baud "$(BAUD)" "$(PORT)"
else ifeq ($(RIOT_TERMINAL),custom)
  # Do nothing. This name is reserved so that TERMFLAGS does not get
  # set if the user wants to override TERMPROG.
endif

RIOT_RAWTERMINAL ?= socat
ifeq ($(RIOT_RAWTERMINAL),socat)
  SOCAT_LOCAL ?= STDIO,icanon=0,echo=0,escape=0x04
  SOCAT_REMOTE ?= open:$(PORT),b$(BAUD),echo=0,raw
  RAWTERMPROG ?= $(RIOT_RAWTERMINAL)
  RAWTERMFLAGS ?= $(SOCAT_LOCAL) $(SOCAT_REMOTE)
else ifeq ($(RIOT_RAWTERMINAL),picocom)
  RAWTERMPROG  ?= picocom
  RAWTERMFLAGS ?= --nolock -q --imap lfcrlf --baud "$(BAUD)" "$(PORT)"
else ifeq ($(RIOT_RAWTERMINAL),custom)
  # Do nothing. This name is reserved so that TERMFLAGS does not get
  # set if the user wants to override TERMPROG.
endif
