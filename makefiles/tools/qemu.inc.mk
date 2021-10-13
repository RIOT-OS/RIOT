QEMU ?= qemu-system-arm
QEMU_MACHINE ?= $(BOARD)
QEMU_MONITOR_PORT ?= 45454
QEMU_MONITOR_FLAGS ?= telnet::$(QEMU_MONITOR_PORT),server,nowait
FLASHFILE ?= $(ELFFILE)

QEMU_SERIAL_TCP_PORT ?= 5555

# Configure emulator variables
EMULATOR ?= $(QEMU)
EMULATOR_FLAGS ?= -machine $(QEMU_MACHINE) -device loader,file=$(ELFFILE) \
                  -serial telnet::$(QEMU_SERIAL_TCP_PORT),server,nowait,nodelay \
                  -monitor $(QEMU_MONITOR_FLAGS) \
                  -nographic

# Configure the qemu terminal access
EMULATOR_SERIAL_PORT ?= /tmp/riot_$(APPLICATION)_$(BOARD)_uart
PORT = $(EMULATOR_SERIAL_PORT)
RIOT_TERMPROG := $(TERMPROG)
RIOT_TERMFLAGS := $(TERMFLAGS)
TERMPROG := $(RIOTTOOLS)/emulator/term.sh
TERMFLAGS := $(RIOT_EMULATOR) $(BOARD) $(APPDIR) $(RIOT_TERMPROG) '$(RIOT_TERMFLAGS)' $(EMULATOR_SERIAL_PORT)

# Configure the debugger
GDB_PORT ?= 3333
QEMU_DEBUG_FLAGS += -S -gdb tcp::$(GDB_PORT)
QEMU_DEBUG_FLAGS += $(EMULATOR_FLAGS)

DEBUGSERVER ?= $(EMULATOR)
DEBUGSERVER_FLAGS ?= $(QEMU_DEBUG_FLAGS)

DEBUGGER_FLAGS ?= $(BOARD) $(APPDIR) $(ELFFILE) $(GDB_PORT)
DEBUGGER ?= $(RIOTTOOLS)/emulator/debug.sh

# No flasher available with qemu emulator
FLASHER ?=
FFLAGS ?=
