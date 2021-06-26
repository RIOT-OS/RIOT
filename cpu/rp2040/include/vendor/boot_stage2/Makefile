all:
	$(CC) $(INCLUDES) -c ../pico-sdk-1.2.0/src/rp2_common/boot_stage2/boot2_generic_03h.S -o boot2_generic_03h.elf
	$(OBJCOPY) -O binary boot2_generic_03h.elf boot2_generic_03h.bin
	../pico-sdk-1.2.0/src/rp2_common/boot_stage2/pad_checksum -s 0xffffffff boot2_generic_03h.bin boot2_generic_03h_pad_checksum.S
	$(CC) $(INCLUDES) -c boot2_generic_03h_pad_checksum.S -o $(BINDIR)/cpu/boot2_generic_03h_pad_checksum.o
