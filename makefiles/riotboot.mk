ifneq (,$(filter riotboot,$(FEATURES_USED)))

.PHONY: riotboot/flash riotboot/flash-bootloader riotboot/flash-slot1 riotboot/flash-slot2 \
        riotboot/verify-image riotboot/bootloader/%

RIOTBOOT ?= $(RIOTBASE)/tools/riotboot/bin/$(BOARD)/riotboot.elf
CFLAGS += -I$(BINDIR)/riotbuild

RIOTBOOT_SECKEY ?= $(RIOTBASE)/dist/tools/firmware/sec.key
RIOTBOOT_PUBKEY ?= $(RIOTBASE)/dist/tools/firmware/pub.key

FIRMWARE_TOOL ?= $(RIOTBASE)/dist/tools/firmware/bin/firmware
RIOTBOOT_HDR_LEN ?= 0x100
# TODO 'firmware' should be rebuilt here at the same time with the same
# compilation options
CFLAGS += -DFIRMWARE_METADATA_SIZE=$(RIOTBOOT_HDR_LEN)

# export variables for 'firmware'
export RIOTBOOT_SLOT0_SIZE
export RIOTBOOT_FW_SLOT_SIZE

APP_ID  ?= 0
APP_VER ?= 0

$(BINDIR)/$(APPLICATION)-%.elf: link
	$(Q)$(_LINK) -o $@

# slot 1 targets
SLOT1_OFFSET := $$(($(RIOTBOOT_SLOT0_SIZE) + $(RIOTBOOT_HDR_LEN)))
SLOT2_OFFSET := $$(($(RIOTBOOT_SLOT0_SIZE) + $(RIOTBOOT_FW_SLOT_SIZE) + $(RIOTBOOT_HDR_LEN)))

$(BINDIR)/$(APPLICATION)-slot%.elf: FW_ROM_LEN=$(RIOTBOOT_FW_SLOT_SIZE - $(RIOTBOOT_HDR_LEN))
$(BINDIR)/$(APPLICATION)-slot1.elf: ROM_OFFSET=$(SLOT1_OFFSET)
$(BINDIR)/$(APPLICATION)-slot2.elf: ROM_OFFSET=$(SLOT2_OFFSET)

# create signed binary target
%.signed.bin: %.sig %.bin
	@echo "creating $@..."
	cat $^ > $@

.PRECIOUS: %.bin
%.sig: %.bin FORCE
	$(Q) $(FIRMWARE_TOOL) sign $< $(APP_VER) $(APP_ID) $$(($(ROM_START_ADDR)+$(OFFSET))) $(RIOTBOOT_SECKEY) - > $@

$(BINDIR)/$(APPLICATION)-slot1.sig: OFFSET=$(SLOT1_OFFSET)
$(BINDIR)/$(APPLICATION)-slot2.sig: OFFSET=$(SLOT2_OFFSET)

# creating pubkey header file
$(RIOTBUILD_CONFIG_HEADER_C): $(BINDIR)/riotbuild/ota_pubkey.h
$(BINDIR)/riotbuild/ota_pubkey.h: $(RIOTBOOT_PUBKEY)
	@mkdir -p $(@D)
	@{ \
		echo "static const unsigned char ota_public_key[] = {"; \
		cat $< | xxd -i ; \
		echo "};"; \
		} > $@

riotboot: $(BINDIR)/$(APPLICATION)-slot1.signed.bin \
          $(BINDIR)/$(APPLICATION)-slot2.signed.bin \
          $(BINDIR)/riotbuild/ota_pubkey.h

riotboot/verify-image:
	$(FIRMWARE_TOOL) verify $(BINDIR)/$(APPLICATION)-slot1.signed.bin $(RIOTBOOT_PUBKEY)
	$(FIRMWARE_TOOL) verify $(BINDIR)/$(APPLICATION)-slot2.signed.bin $(RIOTBOOT_PUBKEY)

riotboot/flash-bootloader: riotboot/bootloader/flash
riotboot/bootloader/%:
	$(Q)/usr/bin/env -i \
		QUIET=$(QUIET)\
		PATH=$(PATH) BOARD=$(BOARD) \
			make --no-print-directory -C $(RIOTBASE)/dist/riotboot $*

BOOTLOADER_BIN = $(RIOTBASE)/dist/riotboot/bin/$(BOARD)
$(BOOTLOADER_BIN)/riotboot.extended.bin: $(BOOTLOADER_BIN)/riotboot.bin
	cp $^ $@.tmp
	truncate -s $$(($(RIOTBOOT_SLOT0_SIZE))) $@.tmp
	mv $@.tmp $@

# Only call sub make if not already in riotboot
ifneq ($(BOOTLOADER_BIN)/riotboot.bin,$(BINFILE))
  $(BOOTLOADER_BIN)/riotboot.bin: riotboot/bootloader/binfile
endif

riotboot/combined-slot1: $(BINDIR)/$(APPLICATION)-slot1-combined.bin
$(BINDIR)/$(APPLICATION)-slot1-combined.bin: $(BOOTLOADER_BIN)/riotboot.extended.bin $(BINDIR)/$(APPLICATION)-slot1.signed.bin
	cat $^ > $@
# edbg
riotboot/flash-combined-slot1: HEXFILE=$(BINDIR)/$(APPLICATION)-slot1-combined.bin
# openocd
riotboot/flash-combined-slot1: export IMAGE_FILE=$(BINDIR)/$(APPLICATION)-slot1-combined.bin
riotboot/flash-combined-slot1: $(BINDIR)/$(APPLICATION)-slot1-combined.bin
	$(FLASHER) $(FFLAGS)

riotboot/flash-slot1: export IMAGE_OFFSET=$(RIOTBOOT_SLOT0_SIZE)
# edbg
riotboot/flash-slot1: HEXFILE=$(BINDIR)/$(APPLICATION)-slot1.signed.bin
# openocd
riotboot/flash-slot1: export IMAGE_FILE=$(BINDIR)/$(APPLICATION)-slot1.signed.bin
riotboot/flash-slot1: $(BINDIR)/$(APPLICATION)-slot1.signed.bin riotboot/flash-bootloader
	$(FLASHER) $(FFLAGS)

# Targets for updates
riotboot/slot1: $(BINDIR)/$(APPLICATION)-slot1.signed.bin
riotboot/slot2: $(BINDIR)/$(APPLICATION)-slot2.signed.bin

# export does not work properly with variables using '$((  ))' so evaluate it in a shell
riotboot/flash-slot2: export IMAGE_OFFSET=$(shell echo $$(($(RIOTBOOT_SLOT0_SIZE) + $(RIOTBOOT_FW_SLOT_SIZE))))
# edbg
riotboot/flash-slot2: HEXFILE=$(BINDIR)/$(APPLICATION)-slot2.signed.bin
# openocd
riotboot/flash-slot2: export IMAGE_FILE=$(BINDIR)/$(APPLICATION)-slot2.signed.bin
riotboot/flash-slot2: $(BINDIR)/$(APPLICATION)-slot2.signed.bin riotboot/flash-bootloader
	$(FLASHER) $(FFLAGS)

riotboot/flash: riotboot/flash-slot1

else
riotboot:
	$(Q)echo "error: riotboot feature not selected! (try FEATURES_REQUIRED += riotboot)"
	$(Q)false

endif # (,$(filter riotboot,$(FEATURES_USED)))
