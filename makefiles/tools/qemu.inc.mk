EMULATOR ?= qemu-system-arm
EMULATOR_MACHINE ?= $(BOARD)
EMULATOR_MONITOR_PORT ?= 45454
EMULATOR_MONITOR_FLAGS ?= telnet::$(EMULATOR_MONITOR_PORT),server,nowait

EMULATOR_FLAGS = -machine $(EMULATOR_MACHINE) -device loader,file=$(ELFFILE) \
                 -serial stdio \
                 -monitor $(EMULATOR_MONITOR_FLAGS) \
                 -nographic
