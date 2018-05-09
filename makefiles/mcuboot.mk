ifdef MCUBOOT_SLOT0_SIZE

IMGTOOL ?= $(RIOTTOOLS)/mcuboot/imgtool.py
override IMGTOOL := $(abspath $(IMGTOOL))

BINFILE ?= $(BINDIR)/$(APPLICATION).bin
SIGN_BINFILE = $(BINDIR)/signed-$(APPLICATION).bin
MCUBOOT_KEYFILE ?= $(BINDIR)/key.pem
MCUBOOT_BIN ?= $(BINDIR)/mcuboot.bin
MCUBOOT_BIN_URL ?= http://download.riot-os.org/mynewt.mcuboot.bin
MCUBOOT_BIN_MD5 ?= 0c71a0589bd3709fc2d90f07a0035ce7

export IMAGE_HDR_SIZE ?= 512

mcuboot-create-key: $(MCUBOOT_KEYFILE)

ifeq ($(BINDIR)/key.pem,$(MCUBOOT_KEYFILE))
$(MCUBOOT_KEYFILE):
	$(Q)mkdir -p $(BINDIR)
	$(Q)$(IMGTOOL) keygen -k $@ -t rsa-2048
endif

mcuboot: mcuboot-create-key link
	@$(COLOR_ECHO)
	@$(COLOR_ECHO) '$(COLOR_PURPLE)Re-linking for MCUBoot at $(MCUBOOT_SLOT0_SIZE)...$(COLOR_RESET)'
	@$(COLOR_ECHO)
	$(Q)$(_LINK) $(LINKFLAGPREFIX)--defsym=offset="$$(($(MCUBOOT_SLOT0_SIZE) + $(IMAGE_HDR_SIZE)))" \
	$(LINKFLAGPREFIX)--defsym=length="$$(($(MCUBOOT_SLOT1_SIZE) - $(IMAGE_HDR_SIZE)))" \
	$(LINKFLAGPREFIX)--defsym=image_header="$(IMAGE_HDR_SIZE)" -o $(ELFFILE) && \
	$(OBJCOPY) $(OFLAGS) -Obinary $(ELFFILE) $(BINFILE) && \
	$(IMGTOOL) sign --key $(MCUBOOT_KEYFILE) --version $(IMAGE_VERSION) --align \
	$(MCUBOOT_IMAGE_ALIGN) -H $(IMAGE_HDR_SIZE) $(BINFILE) $(SIGN_BINFILE)
	@$(COLOR_ECHO)
	@$(COLOR_ECHO) '$(COLOR_PURPLE)Signed with $(MCUBOOT_KEYFILE) for version $(IMAGE_VERSION)\
	$(COLOR_RESET)'
	@$(COLOR_ECHO)

$(MCUBOOT_BIN):
	$(Q)$(DLCACHE) $(MCUBOOT_BIN_URL) $(MCUBOOT_BIN_MD5) $@

.PHONY: mcuboot-flash-bootloader mcuboot-flash

mcuboot-flash-bootloader: HEXFILE = $(MCUBOOT_BIN)
mcuboot-flash-bootloader: $(MCUBOOT_BIN) $(FLASHDEPS)
	FLASH_ADDR=0x0 $(FLASHER) $(FFLAGS)

mcuboot-flash: HEXFILE = $(SIGN_BINFILE)
mcuboot-flash: mcuboot $(FLASHDEPS) mcuboot-flash-bootloader
	FLASH_ADDR=$(MCUBOOT_SLOT0_SIZE) $(FLASHER) $(FFLAGS)

else
mcuboot:
	$(Q)echo "error: mcuboot not supported on board $(BOARD)!"
	$(Q)false
endif # MCUBOOT_SLOT0_SIZE
