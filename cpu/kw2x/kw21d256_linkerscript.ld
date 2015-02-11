OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)

MEMORY
{
    vectors (rx) : ORIGIN = 0x00000000, LENGTH = 0x400
    flashsec (rx) : ORIGIN = 0x00000400, LENGTH = 0x10
    flash (rx)    : ORIGIN = 0x00000410, LENGTH = 256K - 0x410
    sram (rwx)   : ORIGIN = 0x1fffc000, LENGTH = 32K
}

INCLUDE kinetis-noramcode.ld
