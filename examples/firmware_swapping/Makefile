# If no BOARD is found in the environment, use this default:
export BOARD ?= iotlab-m3

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Activate FW slots
FW_SLOTS = 1

FW_METADATA_SPACE = 0x100  # 76 bytes meta-data, 256 byte aligned

# Define the parameters for the FW slot 1
FW_IMAGE1_OFFSET = 0x08004000   # Start at page 8
FW_IMAGE1_LENGTH = 0x3C000  # Reserve 120 pages
FW_IMAGE1_END = 0x08040000 # FW_IMAGE_OFFSET + FW_IMAGE_LENGTH
VERSION_IMG1 = 0x1
UUID_IMG1 = 0xabcd1234

# Define the parameters for the FW slot 1
FW_IMAGE2_OFFSET = 0x08040000   # Start at page 128
FW_IMAGE2_LENGTH = 0x3C000  # Reserve 120 pages
FW_IMAGE1_END = 0x0807C000 # FW_IMAGE_OFFSET + FW_IMAGE_LENGTH
VERSION_IMG2 = 0x2
UUID_IMG2 = 0xabcd1234

bootloader:
	@cd ../bootloader; \
	CFLAGS+=-DFW_METADATA_SPACE=$(FW_METADATA_SPACE) make clean all; \
	cp bin/$(BOARD)/bootloader.hex ../firmware_swapping

gnrc_networking-slot1:
	@cd ../gnrc_networking; \
	CFLAGS="-DFW_SLOT=1 -DVERSION=$(VERSION_IMG1) -DUUID=$(UUID_IMG1) \
	-DFW_SLOTS=$(FW_SLOTS)" FW_SLOTS=$(FW_SLOTS) \
	FW_METADATA_SPACE=$(FW_METADATA_SPACE) FW_SLOT=1 \
	VERSION=$(VERSION_IMG1) UUID=$(UUID_IMG1) \
	make clean all; \
	cp bin/$(BOARD)/slot-image-$(UUID_IMG1)-$(VERSION_IMG1).bin ../firmware_swapping

gnrc_networking-slot2:
	@cd ../gnrc_networking; \
	CFLAGS="-DFW_SLOT=2 -DVERSION=$(VERSION_IMG2) -DUUID=$(UUID_IMG2) \
	-DFW_SLOTS=$(FW_SLOTS)" FW_SLOTS=$(FW_SLOTS) \
	FW_METADATA_SPACE=$(FW_METADATA_SPACE) FW_SLOT=2 \
	VERSION=$(VERSION_IMG2) UUID=$(UUID_IMG2) \
	make clean all; \
	cp bin/$(BOARD)/slot-image-$(UUID_IMG2)-$(VERSION_IMG2).bin ../firmware_swapping

merge-binary:
	srec_cat bootloader.hex -intel -crop 0x08000000 $(FW_IMAGE1_OFFSET) \
	slot-image-$(UUID_IMG1)-$(VERSION_IMG1).bin -binary -offset $(FW_IMAGE1_OFFSET) \
	-crop $(FW_IMAGE1_OFFSET) $(FW_IMAGE1_END) \
	slot-image-$(UUID_IMG2)-$(VERSION_IMG2).bin -binary -offset $(FW_IMAGE2_OFFSET) \
	-crop $(FW_IMAGE2_OFFSET) $(FW_IMAGE2_END) \
	-o firmware-slots.hex -intel

master-hex: bootloader gnrc_networking-slot1 gnrc_networking-slot2 merge-binary
	@true

clean:
	@rm *.hex *.bin

flash:
	OPENOCD_CONFIG=../../boards/$(BOARD)/dist/openocd.cfg \
	HEXFILE=firmware-slots.hex \
	../../dist/tools/openocd/openocd.sh flash