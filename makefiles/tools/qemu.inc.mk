EMULATOR ?= qemu-system-arm
EMULATOR_MACHINE ?= $(BOARD)
EMULATOR_MONITOR_PORT ?= 45454
EMULATOR_MONITOR_FLAGS ?= telnet::$(EMULATOR_MONITOR_PORT),server,nowait
FLASHFILE ?= $(ELFFILE)

EMULATOR_FLAGS = -machine $(EMULATOR_MACHINE) -device loader,file=$(ELFFILE) \
                 -serial stdio \
                 -monitor $(EMULATOR_MONITOR_FLAGS) \
                 -nographic

# Configure the debugger
GDB_PORT ?= 3333
QEMU_DEBUG_FLAGS += -S -gdb tcp::$(GDB_PORT)
QEMU_DEBUG_FLAGS += $(EMULATOR_FLAGS)

DEBUGSERVER ?= $(EMULATOR)
DEBUGSERVER_FLAGS ?= $(QEMU_DEBUG_FLAGS)

DEBUGGER_FLAGS ?= $(BOARD) $(APPDIR) $(ELFFILE) $(GDB_PORT)
DEBUGGER ?= $(RIOTTOOLS)/emulator/debug.sh
