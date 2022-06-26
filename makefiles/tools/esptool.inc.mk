ifeq ($(CPU),esp8266)
  ifneq (,$(filter esp_log_colored,$(USEMODULE)))
    BOOTLOADER_COLOR = _colors
  endif
  ifneq (,$(filter esp_log_startup,$(USEMODULE)))
    BOOTLOADER_INFO = _info
  endif
  # Full path to the bootloader binary. In the ESP32 case this is set by the
  # esp_bootloader module.
  BOOTLOADER_BIN ?= $(RIOTCPU)/$(CPU)/bin/bootloader$(BOOTLOADER_COLOR)$(BOOTLOADER_INFO).bin
endif

ESPTOOL ?= $(RIOTTOOLS)/esptools/esptool.py

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
  FFLAGS += --flash_mode dio --flash_freq $(FLASH_FREQ)
  FFLAGS += --flash_size detect
  FFLAGS += $(BOOTLOADER_POS) $(BOOTLOADER_BIN)
  FFLAGS += 0x8000 $(BINDIR)/partitions.bin
  FFLAGS += 0x10000 $(FLASHFILE)
endif

.PHONY: esp-qemu

esp-qemu:
ifneq (,$(filter esp32,$(CPU)))
	$(Q)echo \
		"--flash_mode $(FLASH_MODE) --flash_freq $(FLASH_FREQ) " \
		"--flash_size $(FLASH_SIZE)MB" \
		"$(BOOTLOADER_POS) $(BOOTLOADER_BIN)" \
		"0x8000 $(BINDIR)/partitions.bin" \
		"0x10000 $(FLASHFILE)" > $(BINDIR)/qemu_flash_args
	$(Q)$(ESPTOOL) \
		--chip $(CPU_FAM) merge_bin \
		--fill-flash-size 4MB \
		-o $(BINDIR)/qemu_flash_image.bin @$(BINDIR)/qemu_flash_args
	$(Q)cp $(RIOTCPU)/$(CPU)/bin/rom_0x3ff90000_0x00010000.bin $(BINDIR)/rom1.bin
	$(Q)cp $(RIOTCPU)/$(CPU)/bin/rom_0x40000000_0x000c2000.bin $(BINDIR)/rom.bin
else
	$(Q)dd if=/dev/zero bs=1M count=$(FLASH_SIZE) | \
	  tr "\\000" "\\377" > tmp.bin && cat tmp.bin | \
		head -c $$(($(BOOTLOADER_POS))) | \
		cat - $(BOOTLOADER_BIN) tmp.bin | \
		head -c $$((0x8000)) | \
		cat - $(BINDIR)/partitions.bin tmp.bin | \
		head -c $$((0x10000)) | \
		cat - $(FLASHFILE) tmp.bin | \
		head -c $(FLASH_SIZE)MB > $(BINDIR)/$(CPU)flash.bin && rm tmp.bin
endif

# reset tool configuration
RESET ?= $(RIOTTOOLS)/esptools/espreset.py
RESET_FLAGS ?= --port $(PROG_DEV)
