ifdef MCUBOOT_SLOT0_SIZE

IMGTOOL ?= $(RIOTTOOLS)/mcuboot/imgtool.py
override IMGTOOL := $(abspath $(IMGTOOL))

BINFILE ?= $(BINDIR)/$(APPLICATION).bin
SIGN_BINFILE = $(BINDIR)/signed-$(APPLICATION).bin
MCUBOOT_KEYFILE ?= $(BINDIR)/key.pem
MCUBOOT_BIN ?= $(BINDIR)/mcuboot.bin
MCUBOOT_BIN_URL ?= http://download.riot-os.org/mynewt.mcuboot.bin
MCUBOOT_BIN_SHA512 ?=

export IMAGE_HDR_SIZE ?= 512

$(MCUBOOT_KEYFILE) $(MCUBOOT_BIN): $(filter clean, $(MAKECMDGOALS))

mcuboot-create-key: $(MCUBOOT_KEYFILE)

ifeq ($(BINDIR)/key.pem,$(MCUBOOT_KEYFILE))
$(MCUBOOT_KEYFILE):
	$(Q)mkdir -p $(BINDIR)
	$(Q)$(IMGTOOL) keygen -k $@ -t rsa-2048
endif

mcuboot: ROM_OFFSET=$$(($(MCUBOOT_SLOT0_SIZE) + $(IMAGE_HDR_SIZE)))
mcuboot: mcuboot-create-key link
	@$(COLOR_ECHO)
	@$(COLOR_ECHO) '$(COLOR_PURPLE)Re-linking for MCUBoot at $(MCUBOOT_SLOT0_SIZE)...$(COLOR_RESET)'
	@$(COLOR_ECHO)
	$(Q)$(_LINK) -o $(ELFFILE) && \
	$(OBJCOPY) $(OFLAGS) -Obinary $(ELFFILE) $(BINFILE) && \
	$(IMGTOOL) sign --key $(MCUBOOT_KEYFILE) --version $(IMAGE_VERSION) --align \
	$(MCUBOOT_IMAGE_ALIGN) -H $(IMAGE_HDR_SIZE) $(BINFILE) $(SIGN_BINFILE)
	@$(COLOR_ECHO)
	@$(COLOR_ECHO) '$(COLOR_PURPLE)Signed with $(MCUBOOT_KEYFILE) for version $(IMAGE_VERSION)\
	$(COLOR_RESET)'
	@$(COLOR_ECHO)

$(MCUBOOT_BIN):
	$(Q)$(DLCACHE) $@ $(MCUBOOT_BIN_URL) $(MCUBOOT_BIN_SHA512)

.PHONY: mcuboot-flash-bootloader mcuboot-flash

mcuboot-flash-bootloader: FLASHFILE = $(MCUBOOT_BIN)
mcuboot-flash-bootloader: export FLASH_ADDR = 0x0
mcuboot-flash-bootloader: $(MCUBOOT_BIN) $(FLASHDEPS)
	$(flash-recipe)

mcuboot-flash: FLASHFILE = $(SIGN_BINFILE)
mcuboot-flash: export FLASH_ADDR = $(MCUBOOT_SLOT0_SIZE)
mcuboot-flash: mcuboot $(FLASHDEPS) mcuboot-flash-bootloader
	$(flash-recipe)

else
mcuboot:
	$(Q)echo "error: mcuboot not supported on board $(BOARD)!"
	$(Q)false
endif # MCUBOOT_SLOT0_SIZE
