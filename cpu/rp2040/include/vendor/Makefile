PKG_NAME=rpi-pico-sdk

PKG_URL=https://github.com/raspberrypi/pico-sdk.git

PKG_VERSION=afc10f3599c27147a6f34781b7102d86f58aa5f6  # v1.1.2

PKG_LICENSE=Apache-2

PKG_SOURCE_DIR = $(RIOTBASE)/cpu/rp2040/include/vendor/rpi-pico-sdk

include $(RIOTBASE)/pkg/pkg.mk

all:
	$(CC) $(INCLUDES) -c rpi-pico-sdk/src/rp2_common/boot_stage2/boot2_generic_03h.S -o boot2_generic_03h.elf
	$(OBJCOPY) -O binary boot2_generic_03h.elf boot2_generic_03h.bin
	rpi-pico-sdk/src/rp2_common/boot_stage2/pad_checksum -s 0xffffffff boot2_generic_03h.bin boot2_generic_03h_pad_checksum.S
	$(CC) $(INCLUDES) -c boot2_generic_03h_pad_checksum.S -o $(BINDIR)/cpu/boot2_generic_03h_pad_checksum.o
