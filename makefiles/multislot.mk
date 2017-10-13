ifdef SLOT0_SIZE

IMGTOOL ?= $(RIOTBASE)/dist/tools/mcuboot/imgtool.py
override IMGTOOL := $(abspath $(IMGTOOL))

BINFILE ?= $(BINDIR)/$(APPLICATION).bin
SIGN_BINFILE = $(BINDIR)/signed-$(APPLICATION).bin
MCUBOOT_KEYFILE ?= $(BINDIR)/key.pem
OFLAGS = -O binary
MCUBOOT_BIN ?= $(BINDIR)/mcuboot.bin
MCUBOOT_BIN_URL ?= http://download.riot-os.org/mynewt.mcuboot.bin
MCUBOOT_BIN_MD5 ?= 0c71a0589bd3709fc2d90f07a0035ce7

create-key: $(MCUBOOT_KEYFILE)

ifeq ($(BINDIR)/key.pem,$(MCUBOOT_KEYFILE))
$(MCUBOOT_KEYFILE):
	$(Q)mkdir -p $(BINDIR)
	$(Q)$(IMGTOOL) keygen -k $@ -t rsa-2048
endif

mcuboot: create-key link
	@$(COLOR_ECHO)
	@$(COLOR_ECHO) '${COLOR_PURPLE}Re-linking for MCUBoot at $(SLOT0_SIZE)...${COLOR_RESET}'
	@$(COLOR_ECHO)
	$(Q)$(_LINK) $(LINKFLAGPREFIX)--defsym=offset="$$(($(SLOT0_SIZE) + $(IMAGE_HDR_SIZE)))" \
	$(LINKFLAGPREFIX)--defsym=length="$$(($(SLOT1_SIZE) - $(IMAGE_HDR_SIZE)))" \
	$(LINKFLAGPREFIX)--defsym=image_header="$(IMAGE_HDR_SIZE)" -o $(ELFFILE) && \
	$(OBJCOPY) $(OFLAGS) $(ELFFILE) $(BINFILE) && \
	$(IMGTOOL) sign --key $(MCUBOOT_KEYFILE) --version $(IMAGE_VERSION) --align \
	$(IMAGE_ALIGN) -H $(IMAGE_HDR_SIZE) $(BINFILE) $(SIGN_BINFILE)
	@$(COLOR_ECHO)
	@$(COLOR_ECHO) '${COLOR_PURPLE}Signed with $(MCUBOOT_KEYFILE) for version $(IMAGE_VERSION)\
	${COLOR_RESET}'
	@$(COLOR_ECHO)

$(MCUBOOT_BIN):
	$(Q)$(DLCACHE) $(MCUBOOT_BIN_URL) $(MCUBOOT_BIN_MD5) $@

.PHONY: flash-bootloader flash-mcuboot

flash-bootloader: HEXFILE = $(MCUBOOT_BIN)
flash-bootloader: $(MCUBOOT_BIN) $(FLASHDEPS)
	FLASH_ADDR=0x0 $(FLASHER) $(FFLAGS)

flash-mcuboot: HEXFILE = $(SIGN_BINFILE)
flash-mcuboot: mcuboot $(FLASHDEPS) flash-bootloader
	FLASH_ADDR=$(SLOT0_SIZE) $(FLASHER) $(FFLAGS)

###############################################################################

ELFSLOT0        := $(ELFFILE:%.elf=%.slot0.elf)
ELFSLOT1        := $(ELFFILE:%.elf=%.slot1.elf)
ELFSLOT2        := $(ELFFILE:%.elf=%.slot2.elf)
BINSLOT0        := $(ELFSLOT0:%.elf=%.bin)
BINSLOT1        := $(ELFSLOT1:%.elf=%.bin)
BINSLOT2        := $(ELFSLOT2:%.elf=%.bin)
IMAGE_SLOT1     := $(ELFFILE:%.elf=%-slot1-$(APP_ID)-$(APP_VERSION).img)
IMAGE_SLOT2     := $(ELFFILE:%.elf=%-slot2-$(APP_ID)-$(APP_VERSION).img)
COMBINED_BIN    := $(ELFFILE:%.elf=%.combined.bin)
BOOTLOADER_BIN  := $(RIOTBASE)/bootloader/bin/$(BOARD)/bootloader.slot0.bin

FIRMWARE_METADATA_SIZE ?= 0x100
FIRMWARE_TOOLS = $(RIOTBASE)/dist/tools/firmware
FIRMWARE       = $(FIRMWARE_TOOLS)/bin/firmware
GENMETA        = $(FIRMWARE) genmeta
GENKEYS        = $(FIRMWARE) genkeys
VERIFY         = $(FIRMWARE) verify

SECKEY ?= $(RIOTBASE)/key.sec
PUBKEY ?= $(RIOTBASE)/key.pub

MAKETARGETS += combined flash-multislot

$(BOOTLOADER_BIN):
	@env -i PATH=$(PATH) BOARD=$(BOARD) make -C $(RIOTBASE)/bootloader bootloader

$(FIRMWARE):
	$(Q)env -i CFLAGS+=-DFIRMWARE_METADATA_SIZE=$(FIRMWARE_METADATA_SIZE) \
	make clean all -C $(FIRMWARE_TOOLS)

$(SECKEY) $(PUBKEY): $(FIRMWARE)
	$(Q)$(GENKEYS) $(SECKEY) $(PUBKEY)

bootloader: link
	@$(_LINK) \
		$(LINKFLAGPREFIX)--defsym=length="$(SLOT0_SIZE)" \
		-o $(ELFSLOT0) && \
		$(OBJCOPY) -Obinary $(ELFSLOT0) $(BINSLOT0) && \
		truncate -s $$(($(SLOT0_SIZE))) $(BINSLOT0)

multislot: $(SECKEY) $(PUBKEY) link $(BOOTLOADER_BIN)
	$(Q)$(_LINK) \
		$(LINKFLAGPREFIX)--defsym=offset="$(SLOT0_SIZE)+$(FIRMWARE_METADATA_SIZE)" \
		$(LINKFLAGPREFIX)--defsym=length="$(SLOT1_SIZE)-$(FIRMWARE_METADATA_SIZE)" \
		-o $(ELFSLOT1) && \
	$(OBJCOPY) -Obinary $(ELFSLOT1) $(BINSLOT1) && \
	$(GENMETA) $(BINSLOT1) $(APP_VERSION) $(APP_ID) $$(($(SLOT0_ADDR)+$(SLOT0_SIZE)+$(FIRMWARE_METADATA_SIZE))) $(SECKEY) $(BINSLOT1).meta && \
	cat $(BINSLOT1).meta $(BINSLOT1) > $(IMAGE_SLOT1) && \
	\
	$(_LINK) \
		$(LINKFLAGPREFIX)--defsym=offset="$(SLOT0_SIZE)+$(SLOT1_SIZE)+$$(($(FIRMWARE_METADATA_SIZE)))" \
		$(LINKFLAGPREFIX)--defsym=length="$(SLOT2_SIZE)-$(FIRMWARE_METADATA_SIZE)" \
		-o $(ELFSLOT2) && \
	$(OBJCOPY) -Obinary $(ELFSLOT2) $(BINSLOT2) && \
	$(GENMETA) $(BINSLOT2) $(APP_VERSION) $(APP_ID) $$(($(SLOT0_ADDR)+$(SLOT0_SIZE)+$(SLOT1_SIZE)+$(FIRMWARE_METADATA_SIZE))) $(SECKEY) $(BINSLOT2).meta && \
	cat $(BINSLOT2).meta $(BINSLOT2) > $(IMAGE_SLOT2) \
	; \

combined: multislot
	$(Q)sh -c 'cat $(BOOTLOADER_BIN) $(IMAGE_SLOT1) > $(COMBINED_BIN)'

.PHONY: flash-multislot verify firmware-tools-clean

ifneq (, $(filter $(BOARD),iotlab-m3))
ifneq (, $(filter flash-multislot, $(MAKECMDGOALS)))
export BINFILE = $(COMBINED_BIN)
FFLAGS = flash-bin
FLASH_ADDR = $(SLOT0_ADDR)
endif
else
flash-multislot: HEXFILE = $(COMBINED_BIN)
endif

flash-multislot: combined $(FLASHDEPS)
	$(FLASHER) $(FFLAGS)

verify: $(FIRMWARE) $(PUBKEY)
	$(Q)$(VERIFY) $(IMAGE_SLOT1) $(PUBKEY)
	$(Q)$(VERIFY) $(IMAGE_SLOT2) $(PUBKEY)

firmware-tools-clean:
	$(Q)env -i make clean -C $(FIRMWARE_TOOLS)

bootloader-clean:
	@env -i PATH=$(PATH) BOARD=$(BOARD) make -C $(RIOTBASE)/bootloader clean

clean-multislot: firmware-tools-clean bootloader-clean

else
mcuboot:
	$(Q)echo "error: mcuboot not supported on board $(BOARD)!"
	$(Q)false

endif # SLOT0_SIZE
