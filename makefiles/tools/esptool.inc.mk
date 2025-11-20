ESPTOOL_VERSION = 5.0.0
ESPTOOL_VENV = $(RIOTTOOLS)/esptools/venv
ESPTOOL = $(ESPTOOL_VENV)/bin/esptool

# ESP-IDF uses dio as flash mode for esptool.py when qout or qio mode are
# configured to always boot in dual SPI mode
ifneq (,$(filter qout qio,$(FLASH_MODE)))
  FLASH_MODE = dio
endif

# flasher configuration
ifneq (,$(filter esp_qemu,$(USEMODULE)))
  FLASHER =
  FFLAGS =
  FLASHDEPS += esp-qemu
else
  PROGRAMMER_SPEED ?= 460800
  FLASHER = $(ESPTOOL)
  FFLAGS += --chip $(FLASH_CHIP) --port $(PROG_DEV) --baud $(PROGRAMMER_SPEED)
  FFLAGS += --before default-reset write-flash -z
  FFLAGS += --flash-mode $(FLASH_MODE) --flash-freq $(FLASH_FREQ)
  FFLAGS += --flash-size detect
  FFLAGS += $(BOOTLOADER_POS) $(BOOTLOADER_BIN)
  FFLAGS += 0x8000 $(BINDIR)/partitions.bin
  FFLAGS += $(FLASHFILE_POS) $(FLASHFILE)
  FLASHDEPS += esptool
endif

.PHONY: esp-qemu esptool

all: esptool

esp-qemu: $(FLASHFILE)
ifeq (esp32,$(CPU))
	$(Q)echo \
		"--flash-mode $(FLASH_MODE) --flash-freq $(FLASH_FREQ) " \
		"--flash-size $(FLASH_SIZE)MB" \
		"$(BOOTLOADER_POS) $(BOOTLOADER_BIN)" \
		"0x8000 $(BINDIR)/partitions.bin" \
		"$(FLASHFILE_POS) $(FLASHFILE)" > $(BINDIR)/qemu_flash_args
	$(Q)$(ESPTOOL) \
		--chip $(CPU_FAM) merge-bin \
		--pad-to-size 4MB \
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
		head -c $$(($(FLASHFILE_POS))) | \
		cat - $(FLASHFILE) tmp.bin | \
		head -c $(FLASH_SIZE)MB > $(BINDIR)/$(CPU)flash.bin && rm tmp.bin
endif

esptool: $(ESPTOOL_VENV)/.version_$(ESPTOOL_VERSION)

$(ESPTOOL_VENV)/.version_$(ESPTOOL_VERSION):
	$(Q)echo "Installing esptool version $(ESPTOOL_VERSION) in a virtual Python environment"
	$(Q)test -d $(ESPTOOL_VENV) || python3 -m venv $(ESPTOOL_VENV)
	$(Q)rm -f $(ESPTOOL_VENV)/.version_*
	$(Q)$(ESPTOOL_VENV)/bin/pip install -q esptool==$(ESPTOOL_VERSION)
	$(Q)touch $(ESPTOOL_VENV)/.version_$(ESPTOOL_VERSION)

# reset tool configuration
RESET ?= $(RIOTTOOLS)/esptools/espreset.py
RESET_FLAGS ?= --port $(PROG_DEV)
