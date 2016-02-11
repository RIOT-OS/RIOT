# name of the application
APPLICATION = app

# Current app directory
APPDIR ?= $(CURDIR)
APPDIR := $(abspath $(APPDIR))/

# If no BOARD is found in the environment, use this default:
export BOARD ?= samr21-xpro

# This has to be the absolute path to the RIOT base directory:
export RIOTBASE ?= $(CURDIR)/../../..

export RIOTBOARD ?= $(RIOTBASE)/boards
export RIOTBOARD := $(abspath $(RIOTBOARD))

export BINDIRBASE ?= $(APPDIR)/bin
export BINDIRBASE := $(abspath $(BINDIRBASE))

export BINDIR ?= $(BINDIRBASE)/$(BOARD)
export BINDIR := $(abspath $(BINDIR))/

export ELFFILE ?= $(BINDIR)$(APPLICATION).elf
export HEXFILE ?= $(ELFFILE:.elf=.hex)

# This has to be the path to the linker script containing folder
export LINKERBASE ?= $(RIOTBASE)/../../../linker
export LINKERBASE := $(abspath $(LINKERBASE))

# define the cpu used by SAMR21 Xplained Pro board
export CPU = samd21
export CPU_MODEL = samr21g18a

# CPU Architecture used
export CPU_ARCH = cortex-m0plus

# set default for CPU_MODEL
export CPU_MODEL ?= $(CPU)

# export the CPU model and architecture
MODEL = $(shell echo $(CPU_MODEL) | tr 'a-z' 'A-Z')
export CFLAGS += -DCPU_MODEL_$(MODEL)
ARCH = $(shell echo $(CPU_ARCH) | tr 'a-z-' 'A-Z_')

CFLAGS_FPU ?= -mfloat-abi=soft
export MCPU ?= $(CPU_ARCH)

# define build specific options
export CFLAGS_CPU   = -mcpu=$(MCPU) -mlittle-endian -mthumb $(CFLAGS_FPU)
ifneq (llvm,$(TOOLCHAIN))
# Clang (observed with v3.7) does not understand  -mno-thumb-interwork, only add if
# not building with LLVM
export CFLAGS_CPU  += -mthumb-interwork
endif

export CFLAGS_STYLE = -std=gnu99 -Wall -Wstrict-prototypes -Werror=implicit-function-declaration -shared
export CFLAGS_DBG   = -ggdb -g3
export CFLAGS_OPT  ?= -Os

export CFLAGS += $(CFLAGS_CPU) $(CFLAGS_STYLE) $(CFLAGS_DBG) $(CFLAGS_OPT)

# Target triple for the build. Use arm-none-eabi if you are unsure.
export TARGET_TRIPLE ?= arm-none-eabi

# default toolchain prefix, defaults to target triple followed by a dash, you
# will most likely not need to touch this.
export PREFIX ?= $(if $(TARGET_TRIPLE),$(TARGET_TRIPLE)-)

# Use TOOLCHAIN environment variable to select the toolchain to use.
# Default: gnu
TOOLCHAIN ?= gnu

export GDBPREFIX = $(PREFIX)
export DBG = $(GDBPREFIX)gdb

# Import all toolchain settings
include $(RIOTBOARD)/Makefile.include.$(TOOLCHAIN)

all: bin

bin: app.elf
	arm-none-eabi-objcopy -O binary app.elf app.bin
	cp app.elf app-unstripped.elf
	arm-none-eabi-strip app.elf
	xxd -i app.elf > app.h

app.elf : app.o
	arm-none-eabi-ld --fatal-warnings -T app.ld -Map=app.map app.o -o app.elf -shared

app.o : app.c
	arm-none-eabi-gcc -Wall $(CFLAGS) --specs=nosys.specs -c -fPIC app.c

clean:
	rm -f app.bin app.h app.elf app-unstripped.elf app.o app.map
