MSPDEBUGFLAGS += -j $(MSPDEBUG_PROGRAMMER)
ifeq ($(strip $(MSPDEBUG_PROGRAMMER)),uif)
  MSPDEBUGFLAGS += -d $(PROG_DEV)
endif
FLASHER ?= mspdebug
FLASHFILE ?= $(HEXFILE)
FFLAGS = $(MSPDEBUGFLAGS) "prog $(FLASHFILE)"

# setup debugger
DEBUGSERVER = $(FLASHER)
DEBUGSERVER_FLAGS = $(MSPDEBUGFLAGS) gdb
DEBUGGER = $(PREFIX)gdb
DEBUGGER_FLAGS = --ex="target remote localhost:2000" --ex "monitor reset halt" --ex load -ex "monitor reset halt"  $(ELFFILE)

# setup reset tool
RESET ?= mspdebug
RESET_FLAGS ?= $(MSPDEBUGFLAGS) reset
