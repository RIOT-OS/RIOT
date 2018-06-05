ifneq (,$(filter riotboot,$(FEATURES_USED)))

.PHONY: riotboot/flash riotboot/flash-bootloader riotboot/flash-slot1 riotboot/flash-slot2 \
        riotboot/verify-image riotboot/bootloader/%

RIOTBOOT ?= $(RIOTBASE)/tools/riotboot/bin/$(BOARD)/riotboot.elf
CFLAGS += -I$(BINDIR)/riotbuild

RIOTBOOT_SECKEY ?= $(RIOTBASE)/dist/tools/firmware/sec.key
RIOTBOOT_PUBKEY ?= $(RIOTBASE)/dist/tools/firmware/pub.key

FIRMWARE_TOOL ?= $(RIOTBASE)/dist/tools/firmware/bin/firmware
RIOTBOOT_HDR_LEN ?= 0x100

APP_ID  ?= 0
APP_VER ?= 0

$(BINDIR)/$(APPLICATION)-%.elf: link
	$(Q)$(_LINK) -o $@

# slot 1 targets
SLOT1_OFFSET := $$(($(RIOTBOOT_SLOT0_SIZE) + $(RIOTBOOT_HDR_LEN)))
SLOT2_OFFSET := $$(($(RIOTBOOT_SLOT0_SIZE) + $(RIOTBOOT_FW_SLOT_SIZE) + $(RIOTBOOT_HDR_LEN)))

$(BINDIR)/$(APPLICATION)-slot%.elf: FW_ROM_LEN=$(RIOTBOOT_FW_SLOT_SIZE)
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

riotboot/flash-slot1: IMAGE_OFFSET=$(RIOTBOOT_SLOT0_SIZE)
# edbg
riotboot/flash-slot1: HEXFILE=$(BINDIR)/$(APPLICATION)-slot1.signed.bin
# openocd
riotboot/flash-slot1: IMAGE_FILE=$(BINDIR)/$(APPLICATION)-slot1.signed.bin
riotboot/flash-slot1: $(BINDIR)/$(APPLICATION)-slot1.signed.bin riotboot/flash-bootloader
	$(FLASHER) $(FFLAGS)

riotboot/flash-slot2: IMAGE_OFFSET=$$(($(RIOTBOOT_SLOT0_SIZE) + $(RIOTBOOT_FW_SLOT_SIZE)))
# edbg
riotboot/flash-slot2: HEXFILE=$(BINDIR)/$(APPLICATION)-slot2.signed.bin
# openocd
riotboot/flash-slot2: IMAGE_FILE=$(BINDIR)/$(APPLICATION)-slot2.signed.bin
riotboot/flash-slot2: $(BINDIR)/$(APPLICATION)-slot2.signed.bin riotboot/flash-bootloader
	$(FLASHER) $(FFLAGS)

riotboot/flash: riotboot/flash-slot1

else
riotboot:
	$(Q)echo "error: riotboot feature not selected! (try FEATURES_REQUIRED += riotboot)"
	$(Q)false

endif # (,$(filter riotboot,$(FEATURES_USED)))
