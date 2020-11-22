EMULATOR ?= qemu-system-arm
EMULATOR_MACHINE ?= $(BOARD)
EMULATOR_MONITOR_PORT ?= 45454
EMULATOR_MONITOR_FLAGS ?= telnet::$(EMULATOR_MONITOR_PORT),server,nowait
FLASHFILE ?= $(ELFFILE)

EMULATOR_FLAGS = -machine $(EMULATOR_MACHINE) -device loader,file=$(ELFFILE) \
                 -serial telnet::5555,server,nowait,nodelay \
                 -monitor $(EMULATOR_MONITOR_FLAGS) \
                 -nographic

# Configure the qemu terminal access
PORT = /tmp/riot_$(APPLICATION)_$(BOARD)_uart
RIOT_TERMPROG := $(TERMPROG)
RIOT_TERMFLAGS := $(TERMFLAGS)
TERMPROG := $(RIOTTOOLS)/emulator/term.sh
TERMFLAGS := $(RIOT_EMULATOR) $(BOARD) $(APPDIR) $(RIOT_TERMPROG) '$(RIOT_TERMFLAGS)' $(PORT)

# Configure the debugger
GDB_PORT ?= 3333
QEMU_DEBUG_FLAGS += -S -gdb tcp::$(GDB_PORT)
QEMU_DEBUG_FLAGS += $(EMULATOR_FLAGS)

DEBUGSERVER ?= $(EMULATOR)
DEBUGSERVER_FLAGS ?= $(QEMU_DEBUG_FLAGS)

DEBUGGER_FLAGS ?= $(BOARD) $(APPDIR) $(ELFFILE) $(GDB_PORT)
DEBUGGER ?= $(RIOTTOOLS)/emulator/debug.sh
