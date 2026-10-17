# manually set some RIOT env vars, so this Makefile can be called stand-alone
RIOTBASE ?= $(abspath $(CURDIR)/../../../..)
BUILD_DIR ?= $(RIOTBASE)/build
PKGDIRBASE ?= $(BUILD_DIR)/pkg
export RIOTBASE BUILD_DIR PKGDIRBASE

PICOSDK_DIR := $(PKGDIRBASE)/picosdk
BOOT2_DIR := $(PICOSDK_DIR)/src/rp2040/boot_stage2

CC := arm-none-eabi-gcc
OBJCOPY := arm-none-eabi-objcopy
PYTHON ?= python3

INCLUDES := \
	-I$(CURDIR)/include \
	-I$(BOOT2_DIR)/asminclude \
	-I$(PICOSDK_DIR)/src/boards/include \
	-I$(PICOSDK_DIR)/src/common/pico_base_headers/include \
	-I$(PICOSDK_DIR)/src/rp2040/hardware_regs/include \
	-I$(PICOSDK_DIR)/src/rp2040/pico_platform/include \
	-I$(PICOSDK_DIR)/src/rp2_common/pico_platform_common/include \
	-I$(PICOSDK_DIR)/src/rp2_common/pico_platform_compiler/include \
	-I$(PICOSDK_DIR)/src/rp2_common/pico_platform_panic/include \
	-I$(PICOSDK_DIR)/src/rp2_common/pico_platform_sections/include \
	#

CFLAGS := -mcpu=cortex-m0plus -mthumb -O3 -DNDEBUG $(INCLUDES)
LDFLAGS := -Wl,--build-id=none --specs=nosys.specs -nostartfiles \
	-Wl,--script=$(BOOT2_DIR)/boot_stage2.ld
CHECKSUM_SEED := 0xffffffff

BOOT2_VARIANTS := \
	boot2_at25sf128a \
	boot2_generic_03h \
	boot2_is25lp080 \
	boot2_usb_blinky \
	boot2_w25q080 \
	boot2_w25x10cl \
	#

.PHONY: all clean picosdk

all: $(BOOT2_VARIANTS:%=%_padded_checksummed.S)

picosdk:
	"$(MAKE)" -C $(RIOTBASE)/pkg/picosdk prepare

clean:
	rm -f *_padded_checksummed.S *.o *.bin *.elf

$(BOOT2_DIR)/%.S: | picosdk
	@true

%.o: $(BOOT2_DIR)/%.S
	$(CC) $(CFLAGS) -c -o $@ $<

%.elf: %.o
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

%.bin: %.elf
	$(OBJCOPY) -Obinary $< $@

%_padded_checksummed.S: %.bin
	$(PYTHON) $(BOOT2_DIR)/pad_checksum -s $(CHECKSUM_SEED) $< $@.tmp
	{ echo "// SPDX-FileCopyrightText: 2019-2021 Raspberry Pi (Trading) Ltd."; \
	  echo "// SPDX-License-Identifier: BSD-3-Clause"; \
	  echo "// origin: https://github.com/raspberrypi/pico-sdk/tree/master/src/rp2040/boot_stage2"; \
	  cat $@.tmp; } > $@
	rm -f $@.tmp
