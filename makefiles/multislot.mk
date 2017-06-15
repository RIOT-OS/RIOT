ifneq (,$(filter mcuboot create-key flash-bootloader flash-mcuboot, $(MAKECMDGOALS)))
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

else
mcuboot:
	$(Q)echo "error: mcuboot not supported on board $(BOARD)!"
	$(Q)false

endif # SLOT0_SIZE
endif # MAKECMDGOALS

ifneq (,$(filter multislot combined flash-multislot keys verify bootloader multislot-clean, $(MAKECMDGOALS)))

ifndef SLOT0_SIZE
$(error Board $(BOARD) does not define multislot parameters!)
endif

ELFBOOTLOADER := $(ELFFILE)
ELFSLOT1      := $(ELFFILE:%.elf=%.slot1.elf)
ELFSLOT2      := $(ELFFILE:%.elf=%.slot2.elf)

BINBOOTLOADER := $(ELFFILE:%.elf=%.bin)
BINSLOT1      := $(ELFSLOT1:%.elf=%.bin)
BINSLOT2      := $(ELFSLOT2:%.elf=%.bin)

COMBINED_BIN := $(ELFFILE:%.elf=%-slot1-$(FW_APPID)-$(FW_VERSION).combined.bin)
IMAGE_SLOT1 := $(ELFFILE:%.elf=%-slot1-$(FW_APPID)-$(FW_VERSION).bin)
IMAGE_SLOT2 := $(ELFFILE:%.elf=%-slot2-$(FW_APPID)-$(FW_VERSION).bin)

FIRMWARE_METADATA_SIZE ?= 256
SLOT0_ADDR_DEC = $(shell printf "%d" $(SLOT0_ADDR))
SLOT0_SIZE_DEC = $(shell printf "%d" $(SLOT0_SIZE))
SLOT1_SIZE_DEC = $(shell printf "%d" $(SLOT1_SIZE))
FIRMWARE_METADATA_SIZE_DEC = $(shell printf "%d" $(FIRMWARE_METADATA_SIZE))
SLOT1_ADDR = $(shell echo 'obase=16; ${SLOT0_ADDR_DEC}+${SLOT0_SIZE_DEC}+${FIRMWARE_METADATA_SIZE_DEC}' | bc)
SLOT2_ADDR = $(shell echo 'obase=16; ${SLOT0_ADDR_DEC}+${SLOT0_SIZE_DEC}+${SLOT1_SIZE_DEC}+${FIRMWARE_METADATA_SIZE_DEC}' | bc)

FIRMWARE_TOOLS = $(RIOTBASE)/dist/tools/firmware_metadata
FIRMWARE       = $(RIOTBASE)/dist/tools/firmware_metadata/bin/firmware
GENMETA        = $(RIOTBASE)/dist/tools/firmware_metadata/bin/firmware genmeta
GENKEYS        = $(RIOTBASE)/dist/tools/firmware_metadata/bin/firmware genkeys
VERIFY         = $(RIOTBASE)/dist/tools/firmware_metadata/bin/firmware verify
PUBKEY_DIR     = $(RIOTBASE)/sys/include/crypto

ifndef_any_of = $(filter undefined,$(foreach v,$(1),$(origin $(v))))

ifneq ($(call ifndef_any_of,PUBKEY SECKEY),)
PUBKEY = $(BINDIR)/ed25519.pub
SECKEY = $(BINDIR)/ed25519.sec
keys: firmware-tools $(PUBKEY_DIR)/ed25519_pub.h
$(PUBKEY_DIR)/ed25519_pub.h:
	$(Q)mkdir -p $(BINDIR)
	@$(COLOR_ECHO)
	@$(COLOR_ECHO) '${COLOR_RED}Providing default crypto keys to current firmware. \
Please provide PUBKEY and SECKEY env variables to use your keys \
${COLOR_RESET}'
	@$(COLOR_ECHO)
	$(Q)$(GENKEYS) $(SECKEY) $(PUBKEY)
	$(Q)cp $(PUBKEY) ./ed25519.pub
	$(Q)xxd -i ed25519.pub > $(PUBKEY_DIR)/ed25519_pub.h
	$(Q)rm ./ed25519.pub
else
keys:$(PUBKEY_DIR)/ed25519_pub.h
$(PUBKEY_DIR)/ed25519_pub.h:
	$(info Using given keys)
	$(Q)cp $(PUBKEY) ./ed25519.pub
	$(Q)xxd -i ed25519.pub > $(PUBKEY_DIR)/ed25519_pub.h
	$(Q)rm ./ed25519.pub
endif # SECKEY

firmware-tools: $(FIRMWARE)
$(FIRMWARE):
	$(Q)env -i PATH=$(PATH) CFLAGS+=-DFIRMWARE_METADATA_SIZE=$(FIRMWARE_METADATA_SIZE) \
	make -C $(FIRMWARE_TOOLS)

ifneq (, $(filter $(USEMODULE),bootloader))
$(info BOOTLOADER BUILD)
BOOTLOADER = 1
else
BOOTLOADER = 0
BOOTLOADER_BIN = $(RIOTBASE)/bootloader/bin/$(BOARD)/bootloader.bin
$(BOOTLOADER_BIN):
	@env -i PATH=$(PATH) BOARD=$(BOARD) make -C $(RIOTBASE)/bootloader clean bootloader
endif

bootloader: link
	@$(_LINK) \
		$(LINKFLAGPREFIX)--defsym=length="$(SLOT0_SIZE)" \
		-o $(ELFBOOTLOADER) && \
		$(OBJCOPY) -Obinary $(ELFBOOTLOADER) $(BINBOOTLOADER) && \
		truncate -s $$(($(SLOT0_SIZE))) $(BINBOOTLOADER)

multislot: keys link $(BOOTLOADER_BIN) firmware-tools
	$(Q)$(_LINK) \
		$(LINKFLAGPREFIX)--defsym=offset="$(SLOT0_SIZE)+$(FIRMWARE_METADATA_SIZE)" \
		$(LINKFLAGPREFIX)--defsym=length="$(SLOT1_SIZE)-$(FIRMWARE_METADATA_SIZE)" \
		-o $(ELFSLOT1) && \
	$(OBJCOPY) -Obinary $(ELFSLOT1) $(BINSLOT1) && \
	$(GENMETA) $(BINSLOT1) $(FW_VERSION) $(FW_APPID) $(SLOT1_ADDR) $(SECKEY) \
	$(BINSLOT1).meta ; \
	\
	$(_LINK) \
		$(LINKFLAGPREFIX)--defsym=offset="$(SLOT0_SIZE)+$(SLOT1_SIZE)+$$(($(FIRMWARE_METADATA_SIZE)))" \
		$(LINKFLAGPREFIX)--defsym=length="$(SLOT2_SIZE)-$(FIRMWARE_METADATA_SIZE)" \
		-o $(ELFSLOT2) && \
	$(OBJCOPY) -Obinary $(ELFSLOT2) $(BINSLOT2) && \
	$(GENMETA) $(BINSLOT2) $(FW_VERSION) $(FW_APPID) $(SLOT2_ADDR) $(SECKEY) \
	$(BINSLOT2).meta ; \

combined: multislot
	$(Q)sh -c 'cat $(BINSLOT1).meta $(BINSLOT1) > $(IMAGE_SLOT1)'
	$(Q)sh -c 'cat $(BINSLOT2).meta $(BINSLOT2) > $(IMAGE_SLOT2)'
	$(Q)sh -c 'cat $(BOOTLOADER_BIN) $(BINSLOT1).meta $(BINSLOT1) > $(COMBINED_BIN)'

.PHONY: flash-multislot verify

verify: firmware-tools $(PUBKEY) combined
	$(Q)$(VERIFY) $(IMAGE_SLOT1) $(PUBKEY)
	$(Q)$(VERIFY) $(IMAGE_SLOT2) $(PUBKEY)

ifneq (, $(filter $(BOARD),iotlab-m3))
export BINFILE = $(COMBINED_BIN)
FFLAGS = flash-bin
FLASH_ADDR = $(SLOT0_ADDR)
else
flash-multislot: HEXFILE = $(COMBINED_BIN)
endif

flash-multislot: combined $(FLASHDEPS)
	$(FLASHER) $(FFLAGS)

firmware-tools-clean:
	make clean distclean -C $(FIRMWARE_TOOLS)

multislot-clean: firmware-tools-clean
	$(Q)rm -rf $(BOOTLOADER_BIN)
	$(Q)rm -rf $(FIRMWARE)
	$(Q)rm -rf $(PUBKEY_DIR)/ed25519_pub.h

endif # multislot targets filter
