# Size of the flash and the RAM that riot-tee reserves for itself at the start
# of each. The application is linked behind it, see Makefile.include. On the
# nRF9160, the SPU protects memory in regions of 32 KiB (flash) and 8 KiB (RAM),
# so the sizes have to be multiples of these.
TEE_FLASH_SIZE := 0x20000
TEE_RAM_SIZE   := 0x16000
