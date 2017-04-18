/*
 * NXP Kinetis CPU with the following memory layout:
 * 256 kB flash @ 0x00000000
 * 64 kB RAM, split into two banks: 32 kB SRAM_L, 32 kB SRAM_U.
 * SRAM_L ends at 0x1fffffff, SRAM_U begins at 0x20000000
 */
OUTPUT_FORMAT ("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH(arm)

MEMORY
{
    vectors (rx)   : ORIGIN = 0x00000000,       LENGTH = 0x400
    flashsec (rx)  : ORIGIN = 0x00000400,       LENGTH = 0x10
    rom (rx)       : ORIGIN = 0x00000410,       LENGTH = 256K - 0x410
    ram (rwx)      : ORIGIN = 0x20000000 - 32K, LENGTH = 64K
}

INCLUDE kinetis.ld
