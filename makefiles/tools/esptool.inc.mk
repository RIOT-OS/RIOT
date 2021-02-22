ifneq (,$(filter esp_log_colored,$(USEMODULE)))
  BOOTLOADER_COLOR = _colors
endif
ifneq (,$(filter esp_log_startup,$(USEMODULE)))
  BOOTLOADER_INFO = _info
endif

BOOTLOADER_BIN = bootloader$(BOOTLOADER_COLOR)$(BOOTLOADER_INFO).bin

ESPTOOL ?= $(RIOTTOOLS)/esptool/esptool.py

# The ELFFILE is the base one used for flashing
FLASHFILE ?= $(ELFFILE)

# configure preflasher to convert .elf to .bin before flashing
PREFLASHER = $(ESPTOOL)
PREFFLAGS  = --chip $(FLASH_CHIP) elf2image
PREFFLAGS += --flash_mode $(FLASH_MODE) --flash_size $(FLASH_SIZE)MB
PREFFLAGS += --flash_freq $(FLASH_FREQ) $(FLASH_OPTS)
PREFFLAGS += -o $(FLASHFILE).bin $(FLASHFILE);
PREFFLAGS += printf "\n" > $(BINDIR)/partitions.csv;
PREFFLAGS += printf "nvs, data, nvs, 0x9000, 0x6000\n" >> $(BINDIR)/partitions.csv;
PREFFLAGS += printf "phy_init, data, phy, 0xf000, 0x1000\n" >> $(BINDIR)/partitions.csv;
PREFFLAGS += printf "factory, app, factory, 0x10000, " >> $(BINDIR)/partitions.csv;
PREFFLAGS += ls -l $(FLASHFILE).bin | awk '{ print $$5 }' >> $(BINDIR)/partitions.csv;

PREFFLAGS += python3 $(RIOTTOOLS)/esptool/gen_esp32part.py
PREFFLAGS += --verify $(BINDIR)/partitions.csv $(BINDIR)/partitions.bin
FLASHDEPS += preflash

# flasher configuration
ifneq (,$(filter esp_qemu,$(USEMODULE)))
  FLASHER = dd
  FFLAGS += if=/dev/zero bs=1M count=$(FLASH_SIZE) |
  FFLAGS += tr "\\000" "\\377" > tmp.bin && cat tmp.bin |
  FFLAGS += head -c $$(($(BOOTLOADER_POS))) |
  FFLAGS += cat - $(RIOTCPU)/$(CPU)/bin/$(BOOTLOADER_BIN) tmp.bin |
  FFLAGS += head -c $$((0x8000)) |
  FFLAGS += cat - $(BINDIR)/partitions.bin tmp.bin |
  FFLAGS += head -c $$((0x10000)) |
  FFLAGS += cat - $(FLASHFILE).bin tmp.bin |
  FFLAGS += head -c $(FLASH_SIZE)MB > $(BINDIR)/$(CPU)flash.bin && rm tmp.bin;
  ifeq (esp32,$(CPU_FAM))
    FFLAGS += cp $(RIOTCPU)/$(CPU)/bin/rom_0x3ff90000_0x00010000.bin $(BINDIR)/rom1.bin &&
    FFLAGS += cp $(RIOTCPU)/$(CPU)/bin/rom_0x40000000_0x000c2000.bin $(BINDIR)/rom.bin
  endif
else
  PROGRAMMER_SPEED ?= 460800
  FLASHER = $(ESPTOOL)
  FFLAGS += --chip $(FLASH_CHIP) --port $(PROG_DEV) --baud $(PROGRAMMER_SPEED)
  FFLAGS += --before default_reset write_flash -z
  FFLAGS += --flash_mode $(FLASH_MODE) --flash_freq $(FLASH_FREQ)
  FFLAGS += $(BOOTLOADER_POS) $(RIOTCPU)/$(CPU)/bin/$(BOOTLOADER_BIN)
  FFLAGS += 0x8000 $(BINDIR)/partitions.bin
  FFLAGS += 0x10000 $(FLASHFILE).bin
endif

# reset tool configuration
RESET ?= $(RIOTTOOLS)/esptool/espreset.py
RESET_FLAGS ?= --port $(PROG_DEV)
