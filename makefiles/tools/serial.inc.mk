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

export BAUD ?= 115200

TERMINAL ?= pyterm

ifeq ($(TERMINAL),pyterm)
    export TERMPROG = $(RIOTBASE)/dist/tools/pyterm/pyterm
    export TERMFLAGS += -p "$(PORT)" -b "$(BAUD)"
else ifeq ($(TERMINAL),picocom)
    export TERMPROG = picocom
    export TERMFLAGS = --nolock --imap lfcrlf --echo --baud "$(BAUD)" "$(PORT)"
endif

export PORT
