QEMU ?= qemu-system-arm
QEMU_MACHINE ?= $(BOARD)
FLASHFILE ?= $(EXECUTABLE)

ifeq (,$(EMULATOR_TMP_DIR))
  EMULATOR_TMP_DIR := $(shell mktemp -td riot_$(APPLICATION)_$(BOARD).XXXXX)
endif

EMULATOR_SERIAL_PORT ?= $(EMULATOR_TMP_DIR)/uart
EMULATOR_MONITOR ?= $(EMULATOR_TMP_DIR)/mon

# Configure emulator variables
EMULATOR ?= $(QEMU)
EMULATOR_FLAGS ?= -machine $(QEMU_MACHINE) \
                  -device loader,file=$(EXECUTABLE) \
                  -serial  unix:$(EMULATOR_SERIAL_PORT)_socket,server=on,wait=off \
                  -monitor unix:$(EMULATOR_MONITOR)_socket,server=on,wait=off \
                  -nographic

# Configure the qemu terminal access
PORT = $(EMULATOR_SERIAL_PORT)
RIOT_TERMPROG := $(TERMPROG)
RIOT_TERMFLAGS := $(TERMFLAGS)
TERMPROG := $(RIOTTOOLS)/emulator/term.sh
TERMFLAGS := $(RIOT_EMULATOR) $(BOARD) $(APPDIR) $(RIOT_TERMPROG) '$(RIOT_TERMFLAGS)' $(EMULATOR_SERIAL_PORT) $(EMULATOR_TMP_DIR)

# Configure the debugger ,wait=off
GDB_REMOTE ?= $(EMULATOR_TMP_DIR)/gdb_socket

QEMU_DEBUG_FLAGS += -S -gdb unix:$(GDB_REMOTE),server=on
QEMU_DEBUG_FLAGS += $(EMULATOR_FLAGS)

DEBUGSERVER ?= $(EMULATOR)
DEBUGSERVER_FLAGS ?= $(QEMU_DEBUG_FLAGS)

DEBUGGER_FLAGS ?= $(BOARD) $(APPDIR) $(DEBUG_EXECUTABLE) $(GDB_REMOTE) $(EMULATOR_TMP_DIR)
DEBUGGER ?= $(RIOTTOOLS)/emulator/debug.sh

# No flasher available with qemu emulator
FLASHER ?=
FFLAGS ?=
