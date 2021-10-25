ifneq (,$(filter esp_log_colored,$(USEMODULE)))
  BOOTLOADER_COLOR = _colors
endif
ifneq (,$(filter esp_log_startup,$(USEMODULE)))
  BOOTLOADER_INFO = _info
endif

BOOTLOADER_BIN = bootloader$(BOOTLOADER_COLOR)$(BOOTLOADER_INFO).bin

ESPTOOL ?= $(RIOTTOOLS)/esptool/esptool.py

# flasher configuration
ifneq (,$(filter esp_qemu,$(USEMODULE)))
  FLASHER =
  FFLAGS =
  FLASHDEPS += esp-qemu
else
  PROGRAMMER_SPEED ?= 460800
  FLASHER = $(ESPTOOL)
  FFLAGS += --chip $(FLASH_CHIP) --port $(PROG_DEV) --baud $(PROGRAMMER_SPEED)
  FFLAGS += --before default_reset write_flash -z
  FFLAGS += --flash_mode $(FLASH_MODE) --flash_freq $(FLASH_FREQ)
  FFLAGS += $(BOOTLOADER_POS) $(RIOTCPU)/$(CPU)/bin/$(BOOTLOADER_BIN)
  FFLAGS += 0x8000 $(BINDIR)/partitions.bin
  FFLAGS += 0x10000 $(FLASHFILE)
endif

.PHONY: esp-qemu

esp-qemu:
	$(Q)dd if=/dev/zero bs=1M count=$(FLASH_SIZE) | \
	  tr "\\000" "\\377" > tmp.bin && cat tmp.bin | \
		head -c $$(($(BOOTLOADER_POS))) | \
		cat - $(RIOTCPU)/$(CPU)/bin/$(BOOTLOADER_BIN) tmp.bin | \
		head -c $$((0x8000)) | \
		cat - $(BINDIR)/partitions.bin tmp.bin | \
		head -c $$((0x10000)) | \
		cat - $(FLASHFILE) tmp.bin | \
		head -c $(FLASH_SIZE)MB > $(BINDIR)/$(CPU)flash.bin && rm tmp.bin
ifeq (esp32,$(CPU_FAM))
	$(Q)cp $(RIOTCPU)/$(CPU)/bin/rom_0x3ff90000_0x00010000.bin $(BINDIR)/rom1.bin
	$(Q)cp $(RIOTCPU)/$(CPU)/bin/rom_0x40000000_0x000c2000.bin $(BINDIR)/rom.bin
endif

# reset tool configuration
RESET ?= $(RIOTTOOLS)/esptool/espreset.py
RESET_FLAGS ?= --port $(PROG_DEV)
