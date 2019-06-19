ifneq (,$(filter riotboot,$(FEATURES_USED)))

.PHONY: riotboot/flash riotboot/flash-bootloader riotboot/flash-slot0 riotboot/flash-slot1 riotboot/bootloader/%

RIOTBOOT_DIR = $(RIOTBASE)/bootloaders/riotboot
RIOTBOOT ?= $(RIOTBOOT_DIR)/bin/$(BOARD)/riotboot.elf
CFLAGS += -I$(BINDIR)/riotbuild

HEADER_TOOL_DIR = $(RIOTBASE)/dist/tools/riotboot_gen_hdr
HEADER_TOOL ?= $(HEADER_TOOL_DIR)/bin/genhdr
BINDIR_APP = $(BINDIR)/$(APPLICATION)

#
export SLOT0_OFFSET SLOT0_LEN SLOT1_OFFSET SLOT1_LEN

# Mandatory APP_VER, set to epoch by default
APP_VER ?= $(shell date +%s)

# Final target for slot 0 with riot_hdr
SLOT0_RIOT_BIN = $(BINDIR_APP)-slot0.riot.bin
SLOT1_RIOT_BIN = $(BINDIR_APP)-slot1.riot.bin
SLOT_RIOT_BINS = $(SLOT0_RIOT_BIN) $(SLOT1_RIOT_BIN)

$(BINDIR_APP)-%.elf: $(BASELIBS)
	$(Q)$(_LINK) -o $@

# Slot 0 and 1 firmware offset, after header
SLOT0_IMAGE_OFFSET := $$(($(SLOT0_OFFSET) + $(RIOTBOOT_HDR_LEN)))
SLOT1_IMAGE_OFFSET := $$(($(SLOT1_OFFSET) + $(RIOTBOOT_HDR_LEN)))

# Link slots ELF *after* riot_hdr and limit the ROM to the slots length
$(BINDIR_APP)-slot0.elf: FW_ROM_LEN=$$((SLOT0_LEN - $(RIOTBOOT_HDR_LEN)))
$(BINDIR_APP)-slot0.elf: ROM_OFFSET=$(SLOT0_IMAGE_OFFSET)
$(BINDIR_APP)-slot1.elf: FW_ROM_LEN=$$((SLOT1_LEN - $(RIOTBOOT_HDR_LEN)))
$(BINDIR_APP)-slot1.elf: ROM_OFFSET=$(SLOT1_IMAGE_OFFSET)

# Create binary target with RIOT header
$(SLOT_RIOT_BINS): %.riot.bin: %.hdr %.bin
	@echo "creating $@..."
	$(Q)cat $^ > $@

# Compile header tool if it doesn't exist, force its compilation in case
# some files changed
$(HEADER_TOOL): FORCE
	@echo "compiling $@..."
	$(Q)/usr/bin/env -i \
		QUIET=$(QUIET) \
		PATH=$(PATH) \
			$(MAKE) --no-print-directory -C $(HEADER_TOOL_DIR) all

# Generate RIOT header and keep the original binary file
# It must be always regenerated in case of any changes, so FORCE
.PRECIOUS: %.bin
%.hdr: $(HEADER_TOOL) %.bin FORCE
	$(Q)$(HEADER_TOOL) generate $< $(APP_VER) $$(($(ROM_START_ADDR)+$(OFFSET))) $(RIOTBOOT_HDR_LEN) - > $@

$(BINDIR_APP)-slot0.hdr: OFFSET=$(SLOT0_IMAGE_OFFSET)
$(BINDIR_APP)-slot1.hdr: OFFSET=$(SLOT1_IMAGE_OFFSET)

# Generic target to create a binary files for both slots
riotboot: $(SLOT_RIOT_BINS)

# riotboot bootloader compile target
riotboot/flash-bootloader: riotboot/bootloader/flash
riotboot/bootloader/%:
	$(Q)/usr/bin/env -i \
		QUIET=$(QUIET)\
		PATH=$(PATH) BOARD=$(BOARD) \
			$(MAKE) --no-print-directory -C $(RIOTBOOT_DIR) $*

# Generate a binary file from the bootloader which fills all the
# allocated space. This is used afterwards to create a combined
# binary with both bootloader and RIOT firmware with header
BOOTLOADER_BIN = $(RIOTBOOT_DIR)/bin/$(BOARD)
$(BOOTLOADER_BIN)/riotboot.extended.bin: $(BOOTLOADER_BIN)/riotboot.bin
	$(Q)cp $^ $@.tmp
	$(Q)truncate -s $$(($(RIOTBOOT_LEN))) $@.tmp
	$(Q)mv $@.tmp $@

# Only call sub make if not already in riotboot
ifneq ($(BOOTLOADER_BIN)/riotboot.bin,$(BINFILE))
  $(BOOTLOADER_BIN)/riotboot.bin: riotboot/bootloader/binfile
endif

# Create combined binary booloader + RIOT firmware with header
RIOTBOOT_COMBINED_BIN = $(BINDIR_APP)-slot0-combined.bin
riotboot/combined-slot0: $(RIOTBOOT_COMBINED_BIN)
$(RIOTBOOT_COMBINED_BIN): $(BOOTLOADER_BIN)/riotboot.extended.bin $(SLOT0_RIOT_BIN)
	$(Q)cat $^ > $@

RIOTBOOT_EXTENDED_BIN = $(BINDIR_APP)-slot0-extended.bin

# Generate a binary file from slot 0 which covers slot 1 riot_hdr
# in order to invalidate slot 1
$(RIOTBOOT_EXTENDED_BIN): $(RIOTBOOT_COMBINED_BIN)
	$(Q)cp $^ $@.tmp
	$(Q)truncate -s $$(($(SLOT0_OFFSET) + $(SLOT0_LEN) + $(RIOTBOOT_HDR_LEN))) $@.tmp
	$(Q)mv $@.tmp $@

# Flashing rule for combined binaries
riotboot/flash-combined-slot0: FLASHFILE=$(RIOTBOOT_COMBINED_BIN)
riotboot/flash-combined-slot0: $(RIOTBOOT_COMBINED_BIN) $(FLASHDEPS)
	$(flash-recipe)

# Flashing rule for extended binaries
riotboot/flash-extended-slot0: FLASHFILE=$(RIOTBOOT_EXTENDED_BIN)
riotboot/flash-extended-slot0: $(RIOTBOOT_EXTENDED_BIN) $(FLASHDEPS)
	$(flash-recipe)

# Flashing rule for slot 0
riotboot/flash-slot0: export IMAGE_OFFSET=$(SLOT0_OFFSET)
riotboot/flash-slot0: FLASHFILE=$(SLOT0_RIOT_BIN)
riotboot/flash-slot0: $(SLOT0_RIOT_BIN) $(FLASHDEPS)
	$(flash-recipe)

# Flashing rule for slot 1
riotboot/flash-slot1: export IMAGE_OFFSET=$(SLOT1_OFFSET)
riotboot/flash-slot1: FLASHFILE=$(SLOT1_RIOT_BIN)
riotboot/flash-slot1: $(SLOT1_RIOT_BIN) $(FLASHDEPS)
	$(flash-recipe)

# Targets to generate only slots binary
riotboot/slot0: $(SLOT0_RIOT_BIN)
riotboot/slot1: $(SLOT1_RIOT_BIN)

# Default flashing rule for bootloader + slot 0
riotboot/flash: riotboot/flash-slot0 riotboot/flash-bootloader

else
riotboot:
	$(Q)echo "error: riotboot feature not selected! (try FEATURES_REQUIRED += riotboot)"
	$(Q)false

endif # (,$(filter riotboot,$(FEATURES_USED)))
