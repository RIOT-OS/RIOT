CC := arm-none-eabi-gcc
OBJCOPY := arm-none-eabi-objcopy
CFLAGS := -mcpu=cortex-m0plus -mthumb -O3 -DNDEBUG -I$(abspath include)
LDFLAGS := -Wl,--build-id=none --specs=nosys.specs -nostartfiles -Wl,--script=boot_stage2.ld
PAD_CHECKSUM := ./pad_checksum
CHECKSUM_SEED := 0xffffffff

.PHONY: clean all

all: \
	boot2_at25sf128a_padded_checksummed.S \
	boot2_generic_03h_padded_checksummed.S \
	boot2_is25lp080_padded_checksummed.S \
	boot2_usb_blinky_padded_checksummed.S \
	boot2_w25q080_padded_checksummed.S \
	boot2_w25x10cl_padded_checksummed.S \
	#

clean:
	rm -f *_padded_checksummed.S *.o *.bin *.elf

%.o: %.S
	$(CC) $(CFLAGS) -c -o $@ $<

%.elf: %.o
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

%.bin: %.elf
	$(OBJCOPY) -Obinary $< $@

%_padded_checksummed.S: %.bin
	$(PAD_CHECKSUM) -s $(CHECKSUM_SEED) $< $@

