.PHONY: mcuboot mcuboot-create-key mcuboot-flash-bootloader mcuboot-flash

ifdef MCUBOOT_SLOT0_SIZE

IMGTOOL ?= $(RIOTTOOLS)/mcuboot/imgtool.py
IMGTOOL_KEYFLAGS ?= keygen -t rsa-2048

SIGN_BINFILE = $(BINDIR)/signed-$(APPLICATION).bin
MCUBOOT_KEYFILE ?= $(BINDIR)/key.pem

MCUBOOT_ELF ?= $(ELFFILE:.elf=.mcuboot.elf)
MCUBOOT_BIN ?= $(MCUBOOT_ELF:.elf=.bin)

MCUBOOT_LOADER_BIN ?= $(BINDIR)/mcuboot.bin
MCUBOOT_LOADER_BIN_URL ?= http://download.riot-os.org/mynewt.mcuboot.bin
MCUBOOT_LOADER_BIN_MD5 ?= 0c71a0589bd3709fc2d90f07a0035ce7

IMAGE_HDR_SIZE ?= 512

$(MCUBOOT_KEYFILE) $(MCUBOOT_LOADER_BIN): $(filter clean, $(MAKECMDGOALS))

mcuboot-create-key: $(MCUBOOT_KEYFILE)

$(BINDIR)/key.pem:
	$(Q)mkdir -p $(BINDIR)
	$(Q)$(IMGTOOL) $(IMGTOOL_KEYFLAGS) -k $@

$(MCUBOOT_ELF): ROM_OFFSET=$$(($(MCUBOOT_SLOT0_SIZE) + $(IMAGE_HDR_SIZE)))
$(MCUBOOT_ELF): $(BASELIBS)
	@$(COLOR_ECHO) '$(COLOR_PURPLE)Re-linking for MCUBoot at $(MCUBOOT_SLOT0_SIZE)...$(COLOR_RESET)'
	$(Q)$(_LINK) -o $@

$(SIGN_BINFILE): $(MCUBOOT_BIN) $(MCUBOOT_KEYFILE)
	@$(COLOR_ECHO) '$(COLOR_PURPLE)Signing with $(MCUBOOT_KEYFILE) for version $(IMAGE_VERSION)$(COLOR_RESET)'
	$(Q)$(IMGTOOL) sign --key $(MCUBOOT_KEYFILE) --version $(IMAGE_VERSION) --align \
	$(MCUBOOT_IMAGE_ALIGN) -H $(IMAGE_HDR_SIZE) $(MCUBOOT_BIN) $(SIGN_BINFILE)

ifeq ($(BUILD_IN_DOCKER),1)
mcuboot: ..in-docker-container
else
mcuboot: $(SIGN_BINFILE)
endif # BUILD_IN_DOCKER

$(MCUBOOT_LOADER_BIN):
	$(Q)$(DLCACHE) $(MCUBOOT_LOADER_BIN_URL) $(MCUBOOT_LOADER_BIN_MD5) $@

mcuboot-flash-bootloader: FLASHFILE = $(MCUBOOT_LOADER_BIN)
mcuboot-flash-bootloader: export FLASH_ADDR = 0x0
mcuboot-flash-bootloader: $(MCUBOOT_LOADER_BIN) $(FLASHDEPS)
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
