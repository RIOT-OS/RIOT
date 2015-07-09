OUTPUT_FORMAT ("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH(arm)

MEMORY
{
    vectors (rx)   : ORIGIN = 0x00000000,       LENGTH = 0x400
    flashsec (rx)  : ORIGIN = 0x00000400,       LENGTH = 0x10
    flash (rx)     : ORIGIN = 0x00000410,       LENGTH = 256K - 0x410
    sram (rwx)     : ORIGIN = 0x20000000 - 32K, LENGTH = 64K
}

INCLUDE kinetis.ld
