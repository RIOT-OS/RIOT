OUTPUT_FORMAT ("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH(arm)

MEMORY
{
    vectors (rx)   : ORIGIN = 0x0,        LENGTH = 0x400
    flashsec (rx)  : ORIGIN = 0x400,      LENGTH = 0x10
    flash (rx)     : ORIGIN = 0x410,      LENGTH = 512K - 0x410
    sram_l (rwx)   : ORIGIN = 0x20000000 - 64K, LENGTH = 64K /* Only accessible via code bus. */
    sram_u (rwx)   : ORIGIN = 0x20000000, LENGTH = 64K /* Only accessible via system bus. */
}

INCLUDE kinetis-base.ld
