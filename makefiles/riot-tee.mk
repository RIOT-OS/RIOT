ifdef SECURE_FLASH_SIZE

SECURE_IMAGE = $(BINDIR)/pkg-build/riot-tee/riot-tee.elf
NON_SECURE_IMAGE = $(BINDIR)/$(APPLICATION).elf
MERGED_HEX = $(BINDIR)/$(APPLICATION)_merged.hex
MERGED_BIN = $(BINDIR)/$(APPLICATION)_merged.bin

MERGEHEX = /usr/bin/mergehex

merged_hex: link
	@echo "[MERGEHEX] $(SECURE_IMAGE) and $(NON_SECURE_IMAGE) -> $(MERGED_HEX)"
	@$(MERGEHEX) -m $(SECURE_IMAGE) $(NON_SECURE_IMAGE) -o $(MERGED_HEX)
	@echo "[OBJCOPY] $(MERGED_HEX) -> $(MERGED_BIN)"
	@objcopy -I ihex -O binary $(MERGED_HEX) $(MERGED_BIN)

.PHONY: tee-flash

tee-flash: FLASHFILE = $(MERGED_HEX)
tee-flash: export FLASH_ADDR = 0x0
tee-flash: merged_hex $(FLASHDEPS)
	$(flash-recipe)
endif
