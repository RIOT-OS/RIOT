# If no BOARD is found in the environment, use this default:
export BOARD ?= iotlab-m3

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..


FIRMWARE ?= gnrc_networking

# Define the parameters for the FW slot 1
VERSION_IMAGE_1 = 0x1
UUID_IMAGE_1 = 0xabcd1234

# Define the parameters for the FW slot 1
VERSION_IMAGE_2 = 0x2
UUID_IMAGE_2 = 0xabcd1234



# # # # # # # # # # # # #
# General configuration #
# # # # # # # # # # # # #

# Activate FW slots
FW_SLOTS = 1

FW_METADATA_SPACE       = 0x100  # 76 bytes meta-data, 256 byte aligned
FIRMWARE_IMAGE_OFFSET_1 = 0x08004000   # Start at page 8
FIRMWARE_IMAGE_OFFSET_2 = 0x08040000   # Start at page 128

# OBJCOPY config (specific for M3 nodes should use
OBJCOPY ?= arm-none-eabi-objcopy
OBJCOPYFLAGS  = --output-target elf32-littlearm
OBJCOPYFLAGS += --binary-architecture=arm

# Configured output by root Makefile.include
# $* will be replaced by the slot number
FIRMWARE_PATH = ../$(FIRMWARE)
FIRMWARE_BIN_PATH = $(FIRMWARE_PATH)/bin/$(BOARD)
FIRMWARE_SLOT_BIN=$(FIRMWARE_BIN_PATH)/slot-image-$(UUID_IMAGE_$*)-$(VERSION_IMAGE_$*).bin


.PHONY: all generate-metadata bootloader-$(FIRMWARE).elf

all: bootloader-$(FIRMWARE).elf

SLOTS = $(FIRMWARE)-slot-1.o $(FIRMWARE)-slot-2.o

bootloader-$(FIRMWARE).elf: $(FIRMWARE)-slot-1.o $(FIRMWARE)-slot-2.o
	CFLAGS+=-DFW_METADATA_SPACE=$(FW_METADATA_SPACE) \
	BASELIBS+=" $(CURDIR)/$(FIRMWARE)-slot-1.o"  \
	BASELIBS+=" $(CURDIR)/$(FIRMWARE)-slot-2.o"  \
	ELFFILE=$(CURDIR)/$@  \
	make -C ../bootloader clean all


$(SLOTS): $(FIRMWARE)-slot-%.o: generate-metadata
	CFLAGS="-DFW_SLOT=$*                      \
			-DVERSION=$(VERSION_IMAGE_$*)     \
			-DUUID=$(UUID_IMAGE_$*)           \
			-DFW_SLOTS=$(FW_SLOTS)"           \
	FW_SLOTS=$(FW_SLOTS) FW_SLOT=$*           \
	FW_METADATA_SPACE=$(FW_METADATA_SPACE)    \
	VERSION=$(VERSION_IMAGE_$*) UUID=$(UUID_IMAGE_$*) \
	make -C $(FIRMWARE_PATH) clean all
	@
	$(OBJCOPY) $(OBJCOPYFLAGS)                        \
		--change-section-vma .data=$(FIRMWARE_IMAGE_OFFSET_$*)\
		--change-section-lma .data=$(FIRMWARE_IMAGE_OFFSET_$*)\
		--rename-section .data=.slot.$*.text          \
		--input-target binary $(FIRMWARE_SLOT_BIN) \
		$@


#
# Version without using the magic with $* to get slot number
#
# FIRMWARE_SLOT_1 = slot-image-$(UUID_IMAGE_1)-$(VERSION_IMAGE_1).bin
# $(FIRMWARE)-slot-1.o:
# 	CFLAGS="-DFW_SLOT=1 -DVERSION=$(VERSION_IMAGE_1) -DUUID=$(UUID_IMAGE_1) \
# 	        -DFW_SLOTS=$(FW_SLOTS)"           \
# 	FW_SLOTS=$(FW_SLOTS) FW_SLOT=1            \
# 	FW_METADATA_SPACE=$(FW_METADATA_SPACE)    \
# 	VERSION=$(VERSION_IMAGE_1) UUID=$(UUID_IMAGE_1) \
# 	make -C $(FIRMWARE_PATH) clean all
# 	@
# 	cp $(FIRMWARE_PATH)/bin/$(BOARD)/$(FIRMWARE_SLOT_1) $(FIRMWARE_SLOT_1)
# 	@
# 	$(OBJCOPY) $(OBJCOPYFLAGS)                        \
# 		--change-section-vma .data=$(FIRMWARE_IMAGE_OFFSET_1)\
# 		--change-section-lma .data=$(FIRMWARE_IMAGE_OFFSET_1)\
# 		--rename-section .data=.slot.1.text           \
# 		--input-target binary $(FIRMWARE_SLOT_1)      \
# 		$@
#
# FIRMWARE_SLOT_2 = slot-image-$(UUID_IMAGE_2)-$(VERSION_IMAGE_2).bin
# $(FIRMWARE)-slot-2.o:
# 	CFLAGS="-DFW_SLOT=2 -DVERSION=$(VERSION_IMAGE_2) -DUUID=$(UUID_IMAGE_2) \
# 	        -DFW_SLOTS=$(FW_SLOTS)"           \
# 	FW_SLOTS=$(FW_SLOTS) FW_SLOT=2            \
# 	FW_METADATA_SPACE=$(FW_METADATA_SPACE)    \
# 	VERSION=$(VERSION_IMAGE_2) UUID=$(UUID_IMAGE_2) \
# 	make -C $(FIRMWARE_PATH) clean all
# 	@
# 	cp $(FIRMWARE_PATH)/bin/$(BOARD)/$(FIRMWARE_SLOT_2) $(FIRMWARE_SLOT_2)
# 	@
# 	$(OBJCOPY) $(OBJCOPYFLAGS)                        \
# 		--change-section-vma .data=$(FIRMWARE_IMAGE_OFFSET_2)\
# 		--change-section-lma .data=$(FIRMWARE_IMAGE_OFFSET_2)\
# 		--rename-section .data=.slot.2.text           \
# 		--input-target binary $(FIRMWARE_SLOT_2)      \
# 		$@


GENERATE_METADA_DIR = ../../dist/tools/firmware_metadata
generate-metadata: $(GENERATE_METADA_DIR)/bin/generate-metadata

$(GENERATE_METADA_DIR)/bin/generate-metadata:
	make -C $(GENERATE_METADA_DIR)

clean:
	@rm -f *.hex *.bin *.elf *.o

flash:
	OPENOCD_CONFIG=../../boards/$(BOARD)/dist/openocd.cfg \
	HEXFILE=firmware-slots.hex \
	../../dist/tools/openocd/openocd.sh flash
