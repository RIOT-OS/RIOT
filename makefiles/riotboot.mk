ifneq (,$(filter riotboot%, $(MAKECMDGOALS)))

.PHONY: riotboot/flash riotboot/flash-bootloader riotboot/flash-slot1 riotboot/flash-slot2 \
        riotboot/verify-image

RIOTBOOT ?= $(RIOTBASE)/tools/riotboot/bin/$(BOARD)/riotboot.elf
CFLAGS += -I$(BINDIR)/riotbuild

RIOTBOOT_SECKEY ?= $(RIOTBASE)/dist/tools/firmware/sec.key
RIOTBOOT_PUBKEY ?= $(RIOTBASE)/dist/tools/firmware/pub.key

FIRMWARE_TOOL ?= $(RIOTBASE)/dist/tools/firmware/bin/firmware
RIOTBOOT_HDR_LEN := 256

APP_ID  ?= 0
APP_VER ?= 0

$(BINDIR)/$(APPLICATION)-%.elf: link
	$(Q) \
		$(_LINK) \
			$(LINKFLAGPREFIX)--defsym=_rom_offset=$(OFFSET) \
			$(LINKFLAGPREFIX)--defsym=_rom_length=$(LENGTH) \
			-o $@

# slot 1 targets
SLOT1_OFFSET := $$(($(RIOTBOOT_SLOT0_SIZE) + $(RIOTBOOT_HDR_LEN)))
SLOT1_LENGTH := $$(($(RIOTBOOT_SLOT1_SIZE) - $(RIOTBOOT_HDR_LEN)))

$(BINDIR)/$(APPLICATION)-slot1.elf: OFFSET=$(SLOT1_OFFSET)
$(BINDIR)/$(APPLICATION)-slot1.elf: LENGTH=$(SLOT1_LENGTH)

# slot 2 targets
SLOT2_OFFSET := $$(($(RIOTBOOT_SLOT0_SIZE) + $(RIOTBOOT_SLOT2_SIZE) + $(RIOTBOOT_HDR_LEN)))
SLOT2_LENGTH := $$(($(RIOTBOOT_SLOT2_SIZE) - $(RIOTBOOT_HDR_LEN)))

$(BINDIR)/$(APPLICATION)-slot2.elf: OFFSET=$(SLOT2_OFFSET)
$(BINDIR)/$(APPLICATION)-slot2.elf: LENGTH=$(SLOT2_LENGTH)

# signing targets
$(BINDIR)/$(APPLICATION)-%.bin:
	@echo "creating $@..."
	$(Q) $(OBJCOPY) -Obinary $< $@.tmp
	$(Q) $(FIRMWARE_TOOL) sign $@.tmp $(APP_VER) $(APP_ID) $(OFFSET) $(RIOTBOOT_SECKEY) - > $@
	$(Q) cat $@.tmp >> $@
	$(Q) rm $@.tmp

$(BINDIR)/$(APPLICATION)-slot1.bin: OFFSET=$(SLOT1_OFFSET)
$(BINDIR)/$(APPLICATION)-slot1.bin: $(BINDIR)/$(APPLICATION)-slot1.elf

$(BINDIR)/$(APPLICATION)-slot2.bin: OFFSET=$(SLOT2_OFFSET)
$(BINDIR)/$(APPLICATION)-slot2.bin: $(BINDIR)/$(APPLICATION)-slot2.elf

# creating pubkey header file
$(RIOTBUILD_CONFIG_HEADER_C): $(BINDIR)/riotbuild/ota_pubkey.h
$(BINDIR)/riotbuild/ota_pubkey.h: $(RIOTBOOT_PUBKEY)
	@mkdir -p $(@D)
	@{ \
		echo "static const unsigned char ota_public_key[] = {"; \
		cat $< | xxd -i ; \
		echo "};"; \
		} > $@

riotboot: $(BINDIR)/$(APPLICATION)-slot1.bin $(BINDIR)/$(APPLICATION)-slot2.bin $(BINDIR)/riotbuild/ota_pubkey.h

riotboot/verify-image:
	$(FIRMWARE_TOOL) verify $(BINDIR)/$(APPLICATION)-slot1.bin $(RIOTBOOT_PUBKEY)
	$(FIRMWARE_TOOL) verify $(BINDIR)/$(APPLICATION)-slot2.bin $(RIOTBOOT_PUBKEY)

riotboot/flash-bootloader:
	$(Q)/usr/bin/env -i \
		PATH=$(PATH) BOARD=$(BOARD) \
			make --no-print-directory -C $(RIOTBASE)/dist/riotboot flash

riotboot/flash-slot1: FLASH_ADDR=$(RIOTBOOT_SLOT0_SIZE)
riotboot/flash-slot1: HEXFILE=$(BINDIR)/$(APPLICATION)-slot1.bin
riotboot/flash-slot1: $(BINDIR)/$(APPLICATION)-slot1.bin riotboot/flash-bootloader
	$(FLASHER) $(FFLAGS)

riotboot/flash-slot2: FLASH_ADDR=$$(($(RIOTBOOT_SLOT0_SIZE) + $(RIOTBOOT_SLOT1_SIZE)))
riotboot/flash-slot2: HEXFILE=$(BINDIR)/$(APPLICATION)-slot2.bin
riotboot/flash-slot2: $(BINDIR)/$(APPLICATION)-slot2.bin riotboot/flash-bootloader
	$(FLASHER) $(FFLAGS)

riotboot/flash: riotboot/flash-slot1

else
riotboot:
	$(Q)echo "error: riotboot not supported on board $(BOARD)!"
	$(Q)false

endif # RIOTBOOT_SLOT0_SIZE
