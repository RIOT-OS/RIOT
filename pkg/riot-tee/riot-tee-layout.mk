# Memory layout of the nRF9160 when riot-tee is used: the secure image owns the
# start of flash and RAM, the application (non-secure image) gets the rest.
#
# The SPU protects flash in 32 KiB and RAM in 8 KiB regions, so both sizes must
# be multiples of these. riot-tee derives its SPU setup from them, and the
# application's flash start follows TEE_FLASH_SIZE. If riot-tee no longer fits,
# its link fails with "region ... overflowed": increase the size by whole regions.
TEE_FLASH_SIZE := 0x20000
TEE_RAM_SIZE   := 0x16000

# RAM of the application, behind the secure RAM. These two values have to be
# updated by hand whenever TEE_RAM_SIZE changes:
#   NS_RAM_START = 0x20000000 + TEE_RAM_SIZE
#   NS_RAM_SIZE  = 0x40000 - TEE_RAM_SIZE
NS_RAM_START   := 0x20016000
NS_RAM_SIZE    := 0x2A000
