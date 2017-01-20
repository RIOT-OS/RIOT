# If no BOARD is found in the environment, use this default:
export BOARD ?= iotlab-m3

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Activate FW slots
FW_SLOTS = 1

# Select slot 1
FW_SLOT = 1

# Give a version to this build
VERSION = 0x0

# Give an UUID
UUID = 0xabcd1234

# Define the parameters for the FW Update
FW_IMAGE_OFFSET = 0x08004000   # Start at page 8
FW_IMAGE_LENGTH = 0x3C000  # Reserve 120 pages
FW_METADATA_SPACE = 0x100  # 108 bytes meta-data, 256 byte aligned
FW_IMAGE_END = 0x08040000 # FW_IMAGE_OFFSET + FW_IMAGE_LENGTH

generate-keys:
	$(RIOTBASE)/dist/tools/nacl_key_generator/bin/./generate-nacl-keys
	
bootloader:
	@cp keys.c ../bootloader
	@cd ../bootloader; \
	CFLAGS+=-DFW_METADATA_SPACE=$(FW_METADATA_SPACE) make clean all; \
	cp bin/$(BOARD)/bootloader.hex ../firmware_swapping

gcoap-slot1:
	@cp firmware_pkey.pub server_skey ../gcoap
	@cd ../gcoap; \
	CFLAGS="-DFW_SLOT=$(FW_SLOT) -DVERSION=$(VERSION) -DUUID=$(UUID) -DFW_SLOTS=$(FW_SLOTS)" \
	FW_SLOTS=$(FW_SLOTS) FW_METADATA_SPACE=$(FW_METADATA_SPACE) FW_SLOT=$(FW_SLOT) \
	FW_IMAGE_OFFSET=$(FW_IMAGE_OFFSET) FW_IMAGE_LENGTH=$(FW_IMAGE_LENGTH) \
	FW_IMAGE_END=$(FW_IMAGE_END) VERSION=$(VERSION) UUID=$(UUID) \
	make clean all; \
	cp bin/$(BOARD)/slot-image-$(UUID)-$(VERSION).bin ../firmware_swapping
	
merge-binary:
	srec_cat bootloader.hex -intel -crop 0x08000000 $(FW_IMAGE_OFFSET) \
	slot-image-$(UUID)-$(VERSION).bin -binary -offset $(FW_IMAGE_OFFSET) \
	-crop $(FW_IMAGE_OFFSET) $(FW_IMAGE_END) \
	-o firmware-slot-$(FW_SLOT).hex -intel

master-hex: generate-keys bootloader gcoap-slot1 merge-binary
	@true
	
clean:
	@rm *.hex *.key firmware* server_skey keys.c *.bin *.pub